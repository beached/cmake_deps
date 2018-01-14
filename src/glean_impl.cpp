// The MIT License (MIT)
//
// Copyright (c) 2016-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <boost/filesystem.hpp>
#include <boost/variant.hpp>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <git2.h>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>

#include <daw/daw_parse_template.h>
#include <daw/daw_string_fmt.h>

#include "git_helper.h"
#include "glean_file.h"
#include "glean_file_parser.h"
#include "glean_impl.h"
#include "glean_options.h"
#include "templates.h"
#include "utilities.h"

namespace daw {
	namespace glean {

		namespace {
			struct dependency_t {
				using child_t = boost::variant<dependency_t, boost::filesystem::path>;
				std::string provides;
				boost::filesystem::path folder;
				std::vector<child_t> children;
				glean_file gf;
			};

			boost::filesystem::path cache_path( glean_item const &item, boost::filesystem::path cache_root ) {
				assert( exists( cache_root ) && is_directory( cache_root ) );
				assert( !item.project_name.empty( ) );
				cache_root /= item.project_name;
				if( item.branch && !item.branch->empty( ) ) {
					cache_root /= *item.branch;
				}
				return cache_root;
			}

			struct item_folders {
				boost::filesystem::path cache;
				boost::filesystem::path build;
				boost::filesystem::path src;
				boost::filesystem::path cmakelist_file;
			}; // item_folders

			item_folders create_cmakelist( glean_item const &item, boost::filesystem::path const &prefix,
			                               glean_config const &cfg ) {

				static std::string const git_template_str = impl::get_git_template( );
				item_folders result;
				result.cache = cache_path( item, cfg.cache_folder );
				verify_folder( result.cache );
				result.build = result.cache / "build";
				verify_folder( result.build );
				result.src = result.cache / "src";
				verify_folder( result.src );
				result.cmakelist_file = result.cache / "CMakeLists.txt";
				verify_file( result.cmakelist_file );
				daw::parse_template git_template{git_template_str};
				git_template.add_callback( "project_name", [&item]( ) -> std::string { return item.project_name; } );
				git_template.add_callback( "git_repo", [&]( ) -> std::string { return *item.uri; } );
				git_template.add_callback( "source_directory", [&]( ) -> std::string { return result.src.string( ); } );
				if( item.branch && !item.branch->empty( ) ) {
					git_template.add_callback( "git_tag", [&]( ) -> std::string { return *item.branch; } );
				} else {
					git_template.add_callback( "git_tag", []( ) -> std::string { return "master"; } );
				}
				git_template.add_callback( "install_directory",
				                           [&]( ) -> std::string { return canonical( prefix ).string( ); } );
				try {
					std::ofstream out_file;
					out_file.open( result.cmakelist_file.string( ), std::ios::out | std::ios::trunc );
					if( !out_file ) {
						throw std::runtime_error( "Could not open file" );
					}
					git_template.to_string( out_file );
					out_file.close( );
				} catch( std::exception const &ex ) {
					std::stringstream ss;
					ss << "Could not write cmake file (" << result.cmakelist_file << "): " << ex.what( );
					throw glean_exception( ss.str( ) );
				}
				return result;
			}

			bool has_glean_file( boost::filesystem::path p ) {
				if( p.empty( ) || !exists( p ) || !is_directory( p ) ) {
					throw std::invalid_argument( "invalid path p" );
				}
				p /= "glean.txt";
				return exists( p ) && is_regular_file( p );
			}

			int git_clone( item_folders const &proj, glean_item const &item, glean_config const &cfg ) {
				if( !item.uri ) {
					throw std::runtime_error( "No URI provided" );
				}
				if( exists( proj.src ) ) {
					remove_all( proj.src );
				}
				create_directory( proj.src );
				git_helper git{};

				return git.clone( *item.uri, proj.src );
			}

			int git_update( item_folders const &proj, glean_item const &item, glean_config const &cfg ) {
				if( !item.uri ) {
					throw std::runtime_error( "No URI provided" );
				}
				git_helper git{};
				if( !exists( proj.src ) ) {
					create_directory( proj.src );
					return git.clone( *item.uri, proj.src );
				}
				return git.update( *item.uri, proj.src );
			}

			int build( item_folders const &proj, glean_item const &item, glean_config const &cfg ) {
				change_directory chd{proj.build};
				{
					int result;
					if( EXIT_SUCCESS != ( result = system( ( cfg.cmake_binary + " .." ).c_str( ) ) ) ) {
						return result;
					}
				}
				return system( ( cfg.cmake_binary + " --build ." ).c_str( ) );
			}

			boost::optional<boost::filesystem::path>
			process_item( glean_item const &item, boost::filesystem::path const &prefix, glean_config const &cfg ) {
				auto cml = create_cmakelist( item, prefix, cfg );
				if( exists( cml.src / ".git" ) ) {
					git_update( cml, item, cfg );
				} else {
					git_clone( cml, item, cfg );
				}
				if( has_glean_file( cml.src ) ) {
					return cml.src / "glean.txt";
				}
				return boost::none;
			}

			dependency_t process_file( std::string const &provides, glean_options const &opts, glean_config const &cfg ) {
				auto depends_obj = parse_cmakes_deps( opts.deps_file( ) );
				dependency_t result{provides, {}};

				for( auto const &dependency : depends_obj.dependencies ) {
					std::cout << "Processing: " << dependency.project_name << '\n';
					try {
						auto new_item = process_item( dependency, opts.prefix( ), cfg );
						if( new_item ) {
							result.children.push_back( std::move( *new_item ) );
						}
					} catch( glean_exception const &ex ) {
						std::cerr << "Error processing: " << dependency.project_name << ":\n" << ex.what( ) << std::endl;
					}
				}
				return result;
			}

			template<typename DepGraph>
			void process_child( std::string const & child, DepGraph & dep_graph,glean_options const &opts, glean_config const &cfg ) {

			}
		} // namespace

		void process_file( glean_options const &opts, glean_config const &cfg ) {

			std::unordered_map<std::string, dependency_t> dep_graph{};
			std::unordered_set<std::string> to_dos{};
			dep_graph["root"] = process_file( "root", opts, cfg );
			for( auto const & c: dep_graph["root"].children ) {

				to_dos.insert(( c.provides ) );
			}

			while( !to_dos.empty( ) ) {

			}
		}
	} // namespace glean
} // namespace daw

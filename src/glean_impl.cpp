// The MIT License (MIT)
//
// Copyright (c) 2016-2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdlib>
#include <exception>
#include <fstream>
#include <git2.h>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <variant>

#include <daw/daw_benchmark.h>
#include <daw/daw_graph.h>
#include <daw/daw_parse_template.h>
#include <daw/daw_string_fmt.h>

#include "git_helper.h"
#include "glean_file.h"
#include "glean_file_parser.h"
#include "glean_impl.h"
#include "glean_options.h"
#include "templates.h"
#include "utilities.h"

namespace daw::glean {
	namespace {
		/*
		struct dependency_t {
		  std::string provides{};
		  fs::path folder{};
		  glean_file gf{};

		  dependency_t( ) = default;
		  dependency_t( daw::string_view prev )
		    : provides( prev.to_string( ) ) {}
		};
		using dep_graph_t = daw::graph_t<std::variant<std::string, dependency_t>>;
		using dep_node_t = typename dep_graph_t::node_t;

		fs::path cache_path( glean_item const &item,
		                                    fs::path cache_root ) {
		  daw::expecting( exists( cache_root ) and is_directory( cache_root ) );
		  daw::expecting( !item.project_name.empty( ) );
		  cache_root /= item.project_name;
		  if( item.branch and !item.branch->empty( ) ) {
		    cache_root /= *item.branch;
		  }
		  return cache_root;
		}

		struct item_folders {
		  fs::path cache;
		  fs::path build;
		  fs::path src;
		  fs::path cmakelist_file;
		}; // item_folders

		item_folders create_cmakelist( glean_item const &item,
		                               fs::path const &prefix,
		                               glean_config const &cfg ) {

		  auto result = item_folders( );

		  result.cache = cache_path( item, cfg.cache_folder );
		  verify_folder( result.cache );
		  result.build = result.cache / "build";
		  verify_folder( result.build );
		  result.src = result.cache / "src";
		  verify_folder( result.src );
		  result.cmakelist_file = result.cache / "CMakeLists.txt";
		  verify_file( result.cmakelist_file );

		  auto git_template = daw::parse_template( impl::get_git_template );
		  git_template.add_callback( "project_name", [&item]( ) -> std::string {
		    return item.project_name;
		  } );
		  git_template.add_callback( "git_repo",
		                             [&]( ) -> std::string { return *item.uri; } );
		  git_template.add_callback( "source_directory", [&]( ) -> std::string {
		    return result.src.string( );
		  } );
		  if( item.branch and !item.branch->empty( ) ) {
		    git_template.add_callback(
		      "git_tag", [&]( ) -> std::string { return *item.branch; } );
		  } else {
		    git_template.add_callback( "git_tag",
		                               []( ) -> std::string { return "master"; } );
		  }
		  git_template.add_callback( "install_directory", [&]( ) -> std::string {
		    return canonical( prefix ).string( );
		  } );
		  try {
		    auto out_file = std::ofstream( );
		    out_file.open( result.cmakelist_file.string( ),
		                   std::ios::out | std::ios::trunc );

		    daw::exception::daw_throw_on_false<std::runtime_error>(
		      out_file, "Could not open file" );
		    git_template.to_string( out_file );
		    out_file.close( );
		  } catch( std::exception const &ex ) {
		    auto ss = std::stringstream( );
		    ss << "Could not write cmake file (" << result.cmakelist_file
		       << "): " << ex.what( );
		    daw::exception::daw_throw<glean_exception>( ss.str( ) );
		  }
		  return result;
		}

		bool has_glean_file( fs::path p ) {
		  daw::exception::daw_throw_on_true<std::invalid_argument>(
		    p.empty( ) or !exists( p ) or !is_directory( p ), "invalid path p" );

		  p /= "glean.txt";
		  return exists( p ) and is_regular_file( p );
		}

		int git_clone( item_folders const &proj, glean_item const &item,
		               glean_config const &cfg ) {
		  daw::exception::daw_throw_on_false<std::runtime_error>(
		    item.uri, "No URI provided" );

		  if( exists( proj.src ) ) {
		    remove_all( proj.src );
		  }
		  create_directory( proj.src );
		  git_helper git{};

		  return git.clone( *item.uri, proj.src );
		}

		int git_update( item_folders const &proj, glean_item const &item,
		                glean_config const &cfg ) {

		  daw::exception::daw_throw_on_false<std::runtime_error>(
		    item.uri, "No URI provided" );

		  git_helper git{};
		  if( !exists( proj.src ) ) {
		    create_directory( proj.src );
		    return git.clone( *item.uri, proj.src );
		  }
		  return git.update( *item.uri, proj.src );
		}

		int build( item_folders const &proj, glean_item const &item,
		           glean_config const &cfg ) {
		  change_directory chd{proj.build};
		  {
		    int result = 0;
		    if( EXIT_SUCCESS !=
		        ( result = system( ( cfg.cmake_binary + " .." ).c_str( ) ) ) ) {
		      return result;
		    }
		  }
		  return system( ( cfg.cmake_binary + " --build ." ).c_str( ) );
		}

		std::optional<fs::path>
		process_item( glean_item const &item, fs::path const &prefix,
		              glean_config const &cfg ) {

		  auto cml = create_cmakelist( item, prefix, cfg );
		  if( exists( cml.src / ".git" ) ) {
		    git_update( cml, item, cfg );
		  } else {
		    git_clone( cml, item, cfg );
		  }
		  if( has_glean_file( cml.src ) ) {
		    return cml.src / "glean.txt";
		  }
		  return std::nullopt;
		}

		std::vector<dependency_t> process_file_impl( dep_graph_t &graph,
		                                             daw::node_id_t root_node_id,
		                                             glean_options const &opts,
		                                             glean_config const &cfg ) {

		  auto depends_obj = parse_cmakes_deps( opts.deps_file( ) );
		  std::vector<dependency_t> result{};

		  for( auto const &dependency : depends_obj.dependencies ) {
		    std::cout << "Processing: " << dependency.project_name << '\n';
		    try {
		      auto new_item = process_item( dependency, opts.prefix( ), cfg );
		      if( new_item ) {
		        result.emplace_back( new_item->generic_string( ) );
		      }
		    } catch( glean_exception const &ex ) {
		      std::cerr << "Error processing: " << dependency.project_name << ":\n"
		                << ex.what( ) << '\n';
		    }
		  }
		  return result;
		}

		template<typename DepGraph>
		void process_child( std::string const &child, DepGraph &dep_graph,
		                    glean_options const &opts, glean_config const &cfg ) {}
		  */
	} // namespace

	/*
void process_file( glean_options const &opts, glean_config const &cfg ) {
	graph_t<std::variant<std::string, dependency_t>> dep_graph{};
	auto root_node = dep_graph.add_node( std::string( "root" ) );
	process_file_impl( dep_graph, root_node, opts, cfg );
	}
	*/
} // namespace daw::glean

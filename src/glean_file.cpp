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

#include <cassert>
#include <optional>
#include <string>
#include <unordered_set>

#include <daw/daw_read_file.h>
#include <daw/json/daw_json_link.h>

#include "daw/glean/build_types.h"
#include "daw/glean/dependency.h"
#include "daw/glean/download_types.h"
#include "daw/glean/glean_file.h"
#include "daw/glean/glean_options.h"
#include "daw/glean/logging.h"

namespace daw::glean {
	namespace {
		template<typename T>
		struct find_dep_by_name_t {
			T *m_deps;

			constexpr find_dep_by_name_t( T &deps ) noexcept
			  : m_deps( &deps ) {}

			constexpr std::optional<daw::node_id_t>
			operator( )( daw::string_view name ) const {
				auto result = m_deps->find( [name]( auto const &cur_node ) {
					return cur_node.value( ).name( ) == name;
				} );
				if( result.empty( ) ) {
					return std::nullopt;
				}
				return result.front( );
			}
		};

		void validate_config_file( glean_config_file const &cfg_file,
		                           fs::path const &config_file_path,
		                           daw::string_view provides ) {
			if( cfg_file.provides != provides ) {
				log_error << "Expected that '" << config_file_path << "' provides '"
				          << provides << "' but '" << cfg_file.provides << "' found\n";
				exit( EXIT_FAILURE );
			}
		}

		void ensure_cache_folder_structure( fs::path const &cache_folder_name ) {
			if( !is_directory( cache_folder_name ) ) {
				fs::create_directories( cache_folder_name / "source" );
				fs::create_directory( cache_folder_name / "build" );
			}
		}

		template<typename T>
		daw::node_id_t
		get_add_node( find_dep_by_name_t<T> const &find_dep_by_name,
		              glean_config_file const &cfg_file, glean_options const &opts,
		              graph_t<dependency> &known_deps,
		              fs::path const &cache_folder_name, bool is_root ) {
			if( auto tmp = find_dep_by_name( cfg_file.provides ); tmp ) {
				return *tmp;
			}
			// New node

			std::string const build_type =
			  is_root ? std::string( "none" ) : cfg_file.build_type;

			return known_deps.add_node(
			  cfg_file.provides, build_types_t( build_type, cache_folder_name,
			                                    opts.install_prefix, opts, true ) );
		}

		template<typename Dep>
		fs::path cache_folder( glean_options const &opts, Dep const &dep ) {
			return opts.glean_cache / dep.build_type / dep.provides;
		}

		daw::node_id_t process_config_file( fs::path const &config_file_path,
		                                    daw::graph_t<dependency> &known_deps,
		                                    glean_options const &opts,
		                                    daw::string_view provides,
		                                    bool is_root = false ) {

			auto const cfg_file = daw::json::from_json<glean_config_file>(
			  daw::read_file( config_file_path.c_str( ) ) );

			validate_config_file( cfg_file, config_file_path, provides );

			auto const cache_folder_name = cache_folder( opts, cfg_file );

			ensure_cache_folder_structure( cache_folder_name );

			auto const find_dep_by_name = find_dep_by_name_t( known_deps );

			auto cur_node_id = get_add_node( find_dep_by_name, cfg_file, opts,
			                                 known_deps, cache_folder_name, is_root );

			if( cfg_file.dependencies.empty( ) or
			    !known_deps.get_node( cur_node_id ).outgoing_edges( ).empty( ) ) {
				return cur_node_id;
			}

			for( glean_file_item const &child_dep : cfg_file.dependencies ) {
				if( auto dep_id = find_dep_by_name( child_dep.provides ); dep_id ) {
					// Child exists in graph
					// Add it as a dependency of current node
					// TODO, handle merging of differences
					known_deps.add_directed_edge( cur_node_id, *dep_id );
					continue;
				}

				auto const dep_cache_folder = cache_folder( opts, child_dep );
				ensure_cache_folder_structure( dep_cache_folder );

				if( download_types_t( child_dep.download_type, child_dep.uri,
				                      child_dep.version, dep_cache_folder )
				      .download( ) == action_status::failure ) {

					log_error << "Error downloading\n";
					exit( EXIT_FAILURE );
				}

				bool const has_glean =
				  exists( dep_cache_folder / "source" / "glean.json" );

				auto builder = build_types_t( child_dep.build_type, dep_cache_folder,
				                              opts.install_prefix, opts, has_glean );

				auto dep_id = known_deps.add_node( child_dep.provides,
				                                   daw::move( builder ), child_dep );
				known_deps.add_directed_edge( cur_node_id, dep_id );

				auto const &cur_dep = known_deps.get_raw_node( dep_id ).value( );
				if( has_glean ) {
					process_config_file( dep_cache_folder / "source" / "glean.json",
					                     known_deps, opts, cur_dep.name( ) );
				}
			}
			return cur_node_id;
		}

		template<typename Function>
		void deps_for_each( daw::graph_t<dependency> known_deps, Function func ) {
			auto leaf_ids = known_deps.find_leaves( );
			while( !leaf_ids.empty( ) ) {
				for( auto leaf_id : leaf_ids ) {
					auto &cur_node = known_deps.get_raw_node( leaf_id );
					auto &cur_dep = cur_node.value( );
					func( cur_dep );
					known_deps.remove_node( leaf_id );
				}
				leaf_ids = known_deps.find_leaves( );
			}
			daw::exception::postcondition_check( known_deps.empty( ) );
		}
	} // namespace

	daw::graph_t<dependency>
	process_config_file( fs::path const &config_file_path,
	                     glean_options const &opts ) {

		if( !exists( config_file_path ) ) {
			log_error << "Could not find config file '" << config_file_path << "'\n";
			exit( EXIT_FAILURE );
		}

		auto known_deps = daw::graph_t<dependency>( );

		auto const cfg_file = daw::json::from_json<glean_config_file>(
		  daw::read_file( config_file_path.c_str( ) ) );

		process_config_file( config_file_path, known_deps, opts, cfg_file.provides,
		                     true );

		return known_deps;
	}

	void process_deps( daw::graph_t<dependency> known_deps,
	                   glean_options const &opts ) {
		deps_for_each( std::move( known_deps ), [&]( dependency const &cur_dep ) {
			if( cur_dep.has_file_dep( ) ) {
				log_message << "-------------------------------------\n";
				log_message << "Processing - " << cur_dep.name( ) << '\n';
				log_message << "-------------------------------------\n\n";
				if( cur_dep.build( opts.build_type ) == action_status::failure ) {
					// Do error stuff
				}
				if( cur_dep.install( opts.build_type ) == action_status::failure ) {
					// Do error stuff
				}
			}
		} );
	}

	void cmake_deps( daw::graph_t<dependency> const &kd ) {
		daw::graph_t<dependency> known_deps = kd;
		struct dep_t {
			std::string name;
			std::string uri;
			std::vector<std::string> depends_on{};
		};
		auto deps = std::vector<dep_t>( );

		auto leaf_ids = known_deps.find_leaves( );
		auto const find_name = [&kd]( node_id_t id ) {
			return kd.get_raw_node( id ).value( ).name( );
		};
		while( !leaf_ids.empty( ) ) {
			for( auto leaf_id : leaf_ids ) {
				auto &cur_node = known_deps.get_raw_node( leaf_id );
				auto &cur_dep = cur_node.value( );
				if( cur_dep.has_file_dep( ) ) {
					auto dep = dep_t{std::string( cur_dep.name( ) ),
					                 std::string( cur_dep.file_dep( ).uri )};
					for( auto child_id : kd.get_node( leaf_id ).outgoing_edges( ) ) {
						auto cur_name = find_name( child_id );
						if( !cur_name.empty( ) ) {
							dep.depends_on.push_back( cur_name );
						}
					}
					deps.push_back( std::move( dep ) );
				}
				known_deps.remove_node( leaf_id );
			}
			leaf_ids = known_deps.find_leaves( );
		}

		log_message << "\ninclude( ExternalProject )\n";
		// need to go backwards so that cmake has correct order
		for( auto const &cur_dep : deps ) {
			log_message << "externalproject_add(\n";
			log_message << "  " << cur_dep.name << "_prj\n";
			if( !cur_dep.depends_on.empty( ) ) {
				log_message << "  DEPENDS";
				for( auto const &child : cur_dep.depends_on ) {
					log_message << ' ' << child << "_prj";
				}
				log_message << '\n';
			}
			log_message << "  GIT_REPOSITORY \"" << cur_dep.uri << "\"\n";
			log_message << "  SOURCE_DIR \"${CMAKE_BINARY_DIR}/dependencies/"
			            << cur_dep.name << "\"\n";
			log_message << "  GIT_TAG \"master\"\n"; // TODO use version
			log_message << "  INSTALL_DIR \"${CMAKE_BINARY_DIR}/install\"\n";
			log_message << "  CMAKE_ARGS "
			               "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install "
			               "-DGLEAN_INSTALL_ROOT=${CMAKE_BINARY_DIR}/install\n";
			log_message << ")\n\n";
		}
		log_message << "include_directories( SYSTEM "
		               "\"${CMAKE_BINARY_DIR}/install/include\" )\n";
		log_message << "link_directories( \"${CMAKE_BINARY_DIR}/install/lib\" )\n";
		log_message << "set( DEP_PROJECT_DEPS";
		for( auto const &dep : deps ) {
			log_message << ' ' << dep.name << "_prj";
		}
		log_message << " )\n";
	}

	glean_file_item::glean_file_item(
	  std::string const &p, std::string const &dt, std::string const &bt,
	  std::string const &u, std::optional<std::string> const &v,
	  std::optional<std::string> const &co,
	  std::optional<std::vector<std::string>> const &ca )

	  : provides( p )
	  , download_type( dt )
	  , build_type( bt )
	  , uri( u )
	  , version( v.value_or( std::string( ) ) )
	  , custom_options( co.value_or( std::string( ) ) )
	  , cmake_args( ca.value_or( std::vector<std::string>( ) ) ) {

		(void)log_message;
	}
} // namespace daw::glean

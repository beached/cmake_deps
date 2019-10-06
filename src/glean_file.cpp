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

#include <daw/daw_graph.h>
#include <daw/daw_graph_algorithm.h>
#include <daw/daw_read_file.h>
#include <daw/json/daw_json_link.h>

#include "daw/glean/build_types.h"
#include "daw/glean/dependency.h"
#include "daw/glean/download_types.h"
#include "daw/glean/glean_file.h"
#include "daw/glean/glean_file_item.h"
#include "daw/glean/glean_options.h"
#include "daw/glean/logging.h"

namespace daw::glean {
	namespace {
		template<typename T>
		struct find_dep_by_name_t {
			T *m_deps;

			constexpr find_dep_by_name_t( T &deps ) noexcept
			  : m_deps( &deps ) {}

			[[nodiscard]] constexpr std::optional<daw::node_id_t>
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
		                           fs::path const &cfg_file_path,
		                           daw::string_view provides ) {
			if( cfg_file.provides != provides ) {
				log_error << "Expected that '" << cfg_file_path << "' provides '"
				          << provides << "' but '" << cfg_file.provides << "' found\n";
				exit( EXIT_FAILURE );
			}
		}

		void ensure_cache_folder_structure( fs::path const &cache_folder_name ) {
			if( not is_directory( cache_folder_name ) ) {
				fs::create_directories( cache_folder_name / "source" );
				fs::create_directory( cache_folder_name / "build" );
			}
		}

		[[nodiscard]] fs::path cache_folder( glean_options const &opts,
		                                     glean_file_item const &dep ) {
			auto const dep_hash =
			  std::to_string( std::hash<glean_file_item>{}( dep ) );
			return opts.glean_cache / dep.provides / dep_hash;
		}

		template<typename T>
		[[nodiscard]] auto
		merge_cfg_item( find_dep_by_name_t<T> const &find_dep_by_name,
		                glean_file_item const &cfg_file, glean_options const &opts,
		                graph_t<dependency> &known_deps,
		                fs::path const &cache_folder_name ) {

			struct result_t {
				bool is_new{};
				::daw::node_id_t node_id{};

				operator bool( ) const noexcept {
					return is_new;
				}
			};
			if( auto tmp = find_dep_by_name( cfg_file.provides ); tmp ) {
				// We already exist by name, lets see if we are the same
				auto &old_node = known_deps.get_raw_node( *tmp );
				if( old_node.value( ).file_dep( ) != cfg_file ) {
					old_node.value( ).add_alternative( cfg_file );
					return result_t{true, *tmp};
				}
				return result_t{false, *tmp};
			}
			// New node
			return result_t{true,
			                known_deps.add_node(
			                  cfg_file.provides,
			                  build_types_t( cfg_file.build_type, cache_folder_name,
			                                 opts.install_prefix, opts, true ),
			                  cfg_file )};
		}
	} // namespace

	[[nodiscard]] action_status downloader( glean_file_item const &child_dep,
	                                        fs::path const &cache_path ) {
		if( not to_bool( download_types_t( child_dep.download_type )
		                   .download( child_dep, cache_path ) ) ) {

			log_error << "Error downloading\n";
			exit( EXIT_FAILURE );
		}
		return action_status::success;
	}

	[[nodiscard]] action_status
	download_node( glean_file_item const &child_dep, fs::path const &cache_folder,
	               ::daw::graph_t<dependency> const &known_deps,
	               glean_options const &opts ) {

		// Check if we have downloaded this resource already
		auto const find_dep_by_name = find_dep_by_name_t( known_deps );
		if( auto node_id = find_dep_by_name( child_dep.provides ); node_id ) {
			auto const &dep = known_deps.get_raw_node( *node_id ).value( );
			if( opts.use_first ) {
				return action_status::success;
			}
			for( auto const &alt : dep.alternatives( ) ) {
				if( not alt.file_dep ) {
					continue;
				}
				auto const &tmp = *alt.file_dep;
				if( child_dep.download_type == tmp.download_type and
				    child_dep.uri == tmp.uri ) {
					return action_status::success;
				}
			}
		}
		return downloader( child_dep, cache_folder );
	}

	[[nodiscard]] bool is_glean_project( fs::path cache_root ) {
		return exists( cache_root / "source" / "glean.json" );
	}

	void merge_nodes( dependency &existing_node, dependency &&new_node ) {}

	::std::optional<::daw::node_id_t> process_config_item(
	  ::daw::graph_t<dependency> &known_deps, glean_options const &opts,
	  glean_file_item const &child_item, ::daw::node_id_t parent_id );

	template<typename T>
	[[nodiscard]] action_status process_dependency(
	  daw::graph_t<dependency> &known_deps, glean_options const &opts,
	  glean_file_item const &child_dep,
	  find_dep_by_name_t<T> const &find_dep_by_name, node_id_t parent_node_id ) {

		auto existing_dep_id = find_dep_by_name( child_dep.provides );

		auto const dep_cache_folder = cache_folder( opts, child_dep );
		ensure_cache_folder_structure( dep_cache_folder );

		if( download_node( child_dep, dep_cache_folder, known_deps, opts ) ==
		      action_status::failure and
		    not child_dep.is_optional ) {
			return action_status::failure;
		}

		bool const has_glean = is_glean_project( dep_cache_folder );

		auto builder = build_types_t( child_dep.build_type, dep_cache_folder,
		                              opts.install_prefix, opts, has_glean );

		auto new_node =
		  dependency( child_dep.provides, daw::move( builder ), child_dep );
		auto const dep_id = [&]( ) {
			if( existing_dep_id ) {
				auto &existing_node = known_deps.get_raw_node( *existing_dep_id );
				merge_nodes( existing_node.value( ), std::move( new_node ) );
				return *existing_dep_id;
			} else {
				return known_deps.add_node( daw::move( new_node ) );
			}
		}( );
		known_deps.add_directed_edge( parent_node_id, dep_id );

		if( has_glean ) {
			(void)process_config_item( known_deps, opts, child_dep, dep_id );
		}
		return action_status::success;
	}

	[[nodiscard]] ::std::optional<::daw::node_id_t> process_config_item(
	  ::daw::graph_t<dependency> &known_deps, glean_options const &opts,
	  glean_file_item const &child_item, ::daw::node_id_t parent_id ) {

		auto const cache_root = cache_folder( opts, child_item );
		ensure_cache_folder_structure( cache_root );

		auto const find_dep_by_name = find_dep_by_name_t( known_deps );

		auto const id = merge_cfg_item( find_dep_by_name, child_item, opts,
		                                known_deps, cache_root );

		if( not id.is_new ) {
			return id.node_id;
		}
		auto const glean_cfg_file = cache_root / "source" / "glean.json";
		if( is_empty( cache_root / "source" ) ) {
			if( not to_bool( downloader( child_item, cache_root ) ) and
			    not child_item.is_optional ) {
				return {};
			}
		}
		if( not exists( glean_cfg_file ) ) {
			return id.node_id;
		}

		auto const glean_cfg_data = ::daw::json::from_json<glean_config_file>(
		  ::daw::read_file( glean_cfg_file.c_str( ) ).value( ) );

		validate_config_file( glean_cfg_data, glean_cfg_file, child_item.provides );

		if( glean_cfg_data.dependencies.empty( ) or not id.is_new ) {
			return id.node_id;
		}
		for( glean_file_item const &child_dep : glean_cfg_data.dependencies ) {
			(void)process_dependency( known_deps, opts, child_dep, find_dep_by_name,
			                          id.node_id );
		}
		return id.node_id;
	}

	[[nodiscard]] daw::graph_t<dependency>
	process_config_file( fs::path const &config_file_path,
	                     glean_options const &opts ) {

		if( not exists( config_file_path ) ) {
			log_error << "Could not find config file '" << config_file_path << "'\n";
			exit( EXIT_FAILURE );
		}

		auto known_deps = daw::graph_t<dependency>( );

		auto const cfg_file = daw::json::from_json<glean_config_file>(
		  daw::read_file( config_file_path.c_str( ) ).value( ) );

		auto const root_node_id = known_deps.add_node(
		  cfg_file.provides,
		  build_types_t( "none", "", opts.install_prefix, opts, false ) );

		for( glean_file_item const &dep : cfg_file.dependencies ) {
			auto child_id =
			  process_config_item( known_deps, opts, dep, root_node_id );
			if( not child_id ) {
				if( dep.is_optional ) {
					continue;
				}
				log_error << "failure to download required dependency\n";
				std::abort( );
			}
			known_deps.add_directed_edge( root_node_id, *child_id );
		}
		return known_deps;
	}

	void process_deps( daw::graph_t<dependency> const &known_deps,
	                   glean_options const &opts ) {

		::daw::reverse_topological_sorted_walk(
		  known_deps, [&opts]( auto const &node ) {
			  auto const &cur_dep = node.value( );
			  if( cur_dep.has_file_dep( ) ) {
				  log_message << "\n-------------------------------------\n";
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

	namespace {
		void output_cmake_item( std::string const &name,
		                        glean_file_item const &fdep,
		                        std::vector<std::string> depends ) {
			log_message << "externalproject_add(\n";
			log_message << "  " << name << "_prj\n";

			if( not depends.empty( ) ) {
				log_message << "  DEPENDS";
				for( auto const &child : depends ) {
					log_message << ' ' << child << "_prj";
				}
				log_message << '\n';
			}
			log_message << "  GIT_REPOSITORY \"" << fdep.uri << "\"\n";
			log_message << "  SOURCE_DIR \"${CMAKE_BINARY_DIR}/dependencies/" << name
			            << "\"\n";

			if( not fdep.version.empty( ) ) {
				log_message << "  GIT_TAG \"" << fdep.version << "\"\n";
			}
			log_message << "  INSTALL_DIR \"${CMAKE_BINARY_DIR}/install\"\n";
			log_message << "  CMAKE_ARGS "
			               "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install "
			               "-DGLEAN_INSTALL_ROOT=${CMAKE_BINARY_DIR}/install\n";
			log_message << ")\n\n";
		}

		template<typename Edges>
		[[nodiscard]] std::vector<std::string>
		get_dependency_names( ::daw::graph_t<dependency> const &kd,
		                      Edges const &edges ) {

			auto const find_name = [&kd]( node_id_t id ) {
				return kd.get_raw_node( id ).value( ).name( );
			};
			auto depends_on = std::vector<std::string>{};
			for( auto const &child_id : edges ) {
				auto cur_name = find_name( child_id );
				if( not cur_name.empty( ) ) {
					depends_on.push_back( cur_name );
				}
			}
			return depends_on;
		}

	} // namespace

	void cmake_deps( daw::graph_t<dependency> const &kd ) {
		if( kd.find_roots( ).size( ) != 1U ) {
			log_error << "There should only ever be 1 root in the graph\n";
			std::abort( );
		}
		log_message << "\ninclude( ExternalProject )\n";

		::daw::reverse_topological_sorted_walk( kd, [&]( auto const &cur_node ) {
			auto const &cur_dep = cur_node.value( );
			if( not cur_dep.has_file_dep( ) ) {
				return;
			}
			output_cmake_item(
			  cur_dep.name( ), cur_dep.file_dep( ),
			  get_dependency_names( kd, cur_node.outgoing_edges( ) ) );
		} );
		log_message << "include_directories( SYSTEM "
		               "\"${CMAKE_BINARY_DIR}/install/include\" )\n";
		log_message << "link_directories( \"${CMAKE_BINARY_DIR}/install/lib\" )\n";
		log_message << "set( DEP_PROJECT_DEPS";

		kd.visit( [&]( auto &&cur_node ) {
			dependency const &cur_dep = cur_node.value( );
			if( not cur_dep.has_file_dep( ) ) {
				return;
			}
			log_message << ' ' << cur_dep.name( ) << "_prj";
		} );
		log_message << " )\n";
	}
} // namespace daw::glean

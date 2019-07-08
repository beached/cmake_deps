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

#include "build_types.h"
#include "dependency.h"
#include "download_types.h"
#include "glean_file.h"
#include "glean_options.h"

namespace daw::glean {
	namespace {
		namespace impl {
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
		} // namespace impl

		daw::node_id_t process_config_file( fs::path const &config_file_path,
		                                    daw::graph_t<dependency> &known_deps,
		                                    glean_options const &opts ) {

			auto cfg_file = daw::json::from_json<glean_config_file>(
			  daw::read_file( config_file_path.string( ) ) );

			auto const find_dep_by_name = impl::find_dep_by_name_t( known_deps );
			if( auto tmp = find_dep_by_name( cfg_file.provides ); tmp ) {
				return *tmp;
			}
			auto const cache_folder_name =
			  opts.glean_cache( ) / cfg_file.build_type / cfg_file.provides;

			if( !is_directory( cache_folder_name ) ) {
				fs::create_directories( cache_folder_name / "source" );
				fs::create_directory( cache_folder_name / "build" );
			}
			auto cur_node_id = known_deps.add_node(
			  cfg_file.provides,
			  build_types_t( cfg_file.build_type, cache_folder_name / "source",
			                 cache_folder_name / "build", opts.install_prefix( ) ),
			  download_none{} );

			for( glean_file_item &child_dep : cfg_file.dependencies ) {
				if( auto dep_id = find_dep_by_name( child_dep.name ); dep_id ) {
					// Child exists in graph
					// Add it as a dependency of current node
					known_deps.add_directed_edge( cur_node_id, *dep_id );
					continue;
				}
				// For now we only support git/cmake
				assert( child_dep.download_type == "git" );
				assert( child_dep.build_type == "cmake" );

				if( !child_dep.version ) {
					child_dep.version = "";
				}
				auto const dep_cache_folder_name =
				  opts.glean_cache( ) / child_dep.build_type / child_dep.name;

				if( !is_directory( dep_cache_folder_name ) ) {
					fs::create_directories( dep_cache_folder_name / "source" );
					fs::create_directory( dep_cache_folder_name / "build" );
				}
				auto downloader = download_types_t( child_dep.download_type,
				                                    child_dep.uri, *child_dep.version,
				                                    dep_cache_folder_name / "source" );
				auto builder = build_types_t(
				  child_dep.build_type, dep_cache_folder_name / "source",
				  dep_cache_folder_name / "build", opts.install_prefix( ) );

				auto dep_id = known_deps.add_node( child_dep.name, daw::move( builder ),
				                                   daw::move( downloader ) );
				known_deps.add_directed_edge( cur_node_id, dep_id );

				auto &cur_dep = known_deps.get_raw_node( dep_id ).value( );
				if( cur_dep.download( ) != action_status::success ) {
					// Error downloading
					std::abort( );
				}
				if( cur_dep.dep_count( ) > 0 ) {
					process_config_file( cur_dep.glean_file( ), known_deps, opts );
				}
			}
			return cur_node_id;
		}
	} // namespace

	void process_config_file( fs::path const &config_file_path,
	                          glean_options const &opts ) {

		if( !exists( config_file_path ) ) {
			return;
		}
		auto known_deps = daw::graph_t<dependency>( );
		process_config_file( config_file_path, known_deps, opts );

		auto leaf_ids = known_deps.find_leaves( );
		while( !leaf_ids.empty( ) ) {
			for( auto leaf_id : leaf_ids ) {
				auto &cur_dep = known_deps.get_raw_node( leaf_id ).value( );
				if( cur_dep.build( opts.build_type( ) ) == action_status::failure ) {
					// Do error stuff
				}
				if( cur_dep.install( opts.build_type( ) ) == action_status::failure ) {
					// Do error stuff
				}
				known_deps.remove_node( leaf_id );
			}
			leaf_ids = known_deps.find_leaves( );
		}
		daw::exception::postcondition_check( known_deps.empty( ) );
	}
} // namespace daw::glean

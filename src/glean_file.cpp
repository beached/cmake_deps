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

#include <array>
#include <cassert>
#include <optional>
#include <string>
#include <unordered_set>
#include <variant>

#include <daw/daw_graph.h>
#include <daw/daw_poly_value.h>
#include <daw/daw_read_file.h>
#include <daw/json/daw_json_link.h>

#include "build_types.h"
#include "dependency.h"
#include "download_types.h"
#include "glean_file.h"

namespace daw::glean {
	/*
	std::string to_string( download_type_t t ) {
	  static std::array<std::string, 4> const result = {
	    {"none", "git", "uri", "github"}};
	  return result[static_cast<uint8_t>( t )];
	}

	download_type_t download_type_from_string( std::string const &str ) {
	  static std::unordered_map<std::string, download_type_t> const result = {
	    {"none", download_type_t::none},
	    {"git", download_type_t::git},
	    {"uri", download_type_t::uri},
	    {"github", download_type_t::github}};
	  return result.at( str );
	}

	std::ostream &operator<<( std::ostream &os, download_type_t const &item ) {
	  return ( os << to_string( item ) );
	}
	*/
	namespace {
		void process_config_file( std::string config_file_path,
		                          std::unordered_set<dependency> &known_deps,
		                          std::string prefix ) {

			auto cfg_file = daw::json::from_json<glean_config_file>(
			  daw::read_file( config_file_path ) );

			auto const name_exists = [&known_deps]( auto &&name ) {
				return std::find_if( known_deps.begin( ), known_deps.end( ),
				                     [&name]( auto const &v ) {
					                     return v.name( ) == name;
				                     } ) != known_deps.end( );
			};
			for( glean_file_item &dep : cfg_file.dependencies ) {

				assert( dep.download_type == "git" );
				assert( dep.build_type == "cmake" );

				if( !dep.version ) {
					dep.version = "";
				}

				if( !name_exists( dep.name ) ) {
					auto downloader = download_types_t( dep.download_type, dep.uri,
					                                    *dep.version, prefix );
					auto builder = build_types_t(
					  dep.build_type, prefix, prefix + "/build", prefix + "/install" );

					auto current_dep = dependency( dep.name, daw::move( builder ),
					                               daw::move( downloader ) );

					current_dep.download( );
					if( current_dep.dep_count( ) > 0 ) {
						process_config_file( current_dep.glean_file( ), known_deps,
						                     prefix );
					}
					known_deps.emplace( std::move( current_dep ) );
				}
			}
		}
	} // namespace

	std::unordered_set<dependency>
	process_config_file( std::string config_file_path, std::string prefix ) {
		auto known_deps = std::unordered_set<dependency>( );

		process_config_file( config_file_path, known_deps, std::move( prefix ) );
		auto const find_leaves = [&known_deps]( )
		  -> std::optional<typename decltype( known_deps )::iterator> {
			auto result = std::find_if(
			  std::begin( known_deps ), std::end( known_deps ),
			  []( dependency const &l ) { return !l.dep_names( ).empty( ); } );
			if( result == known_deps.end( ) ) {
				return {};
			}
			return result;
		};

		auto result = std::unordered_set<dependency>( );
		auto pos = find_leaves( );
		while( pos or *pos == known_deps.end( ) ) {
			auto it = *pos;
			if( it->build( ) == action_status::failure ) {
				// Do error stuff
			}
			if( it->install( ) == action_status::failure ) {
				// Do error stuff
			}
			result.insert( known_deps.extract( *pos ) );
			pos = find_leaves( );
		}
		if( !known_deps.empty( ) ) {
			// Do error stuff
		}
		return result;
	}

	/*
	glean_item::glean_item( download_type_t Type, std::string ProjectName,
	                        std::optional<std::string> Uri,
	                        std::optional<std::string> Branch,
	                        std::optional<std::string> DecompressCommand,
	                        std::optional<std::string> BuildCommand,
	                        std::optional<std::string> InstallCommand )
	  : type( Type )
	  , project_name( std::move( ProjectName ) )
	  , uri( std::move( Uri ) )
	  , branch( std::move( Branch ) )
	  , decompress_command( std::move( DecompressCommand ) )
	  , build_command( std::move( BuildCommand ) )
	  , install_command( std::move( InstallCommand ) ) {}
	  */
} // namespace daw::glean

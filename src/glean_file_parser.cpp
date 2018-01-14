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
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <daw/daw_parser_helper.h>
#include <daw/daw_parser_addons.h>
#include <daw/kv_file.h>

#include "glean_impl.h"
#include "glean_file.h"
#include "glean_file_parser.h"
#include "utilities.h"

namespace daw {
	namespace glean {
		namespace {
			auto split( std::string const & str, std::string const & on ) {
				std::vector<size_t> results;
				if( on.empty( ) || str.empty( ) ) {
					return results;
				}
				size_t pos = 0;
				while( (pos = str.find_first_of( on, pos )) != std::string::npos ) {
					results.push_back( pos );
					pos += on.size( );
				}
				return results;
			}
		}

		template<class Container>
		bool begins_with( Container const & input, Container const & match ) {
			return input.size( ) >= match.size( ) && std::equal( match.begin( ), match.end( ), input.begin( ) );
		}

		std::pair<std::string, std::string> parse_line( std::string const & line ) {
			auto pos = line.find_first_of( '=' );
			if( pos == std::string::npos ) {
				throw glean_exception( "Error on line, no '=' symbol: " + line );
			}
			auto quotes = split( line.substr( pos ) , "\"" );
			if( quotes.size( ) != 2 ) {
				throw glean_exception( "Error on line, value isn't quoted properly" + line );
			}
			std::string value;
			if( pos == 0 ) {
				throw glean_exception( "Error on line, empty key name" + line );
			}
			auto key = boost::trim_copy( line.substr( 0, pos ) );
			if( quotes[1] - quotes[0] > 1 ) {
				value = boost::trim_copy( line.substr( pos + quotes[0] + 1, (quotes[1] - quotes[0]) - 1) );
			}

			if( key.empty() ) {
				throw glean_exception( "Error on line, empty key name" + line );
			}
			return std::make_pair( key, value );
		}

		glean_file parse_cmakes_deps( boost::filesystem::path const & deps_file ) {
			glean_file result;
			glean_item cur_item;
			bool in_item = false;
			for( auto const & kv: daw::kv_file{ deps_file.string( ) } ) {
				if( kv.key == "project_name" ) {
					if( in_item ) {
						result.dependencies.push_back( cur_item );
					}
					in_item = true;
					cur_item = glean_item{ };
					cur_item.project_name = kv.value;
				} else if( in_item ) {
					if( "uri" == kv.key ) {
						cur_item.uri = kv.value;
					} else if( "branch" == kv.key ) {
						cur_item.branch = kv.value;
					} else if( "decompress_command" == kv.key ) {
						cur_item.decompress_command = kv.value;
					} else if( "build_command" == kv.key ) {
						cur_item.build_command = kv.value;
					} else if( "install_command" == kv.key ) {
						cur_item.install_command = kv.value;
					} else if( "type" == kv.key ) {
						cur_item.type = daw::glean::items_type_from_string( kv.value );
					} else {
						throw glean_exception( "Unknown key: '" + kv.key + "'" );
					}
				} else {
					throw glean_exception( "Error in dependencies file key specified without a project_name line first: '" + kv.key + "'" );
				}
			}
			if( in_item && !cur_item.project_name.empty( ) ) {
				result.dependencies.push_back( cur_item );
			}
			return result;
		}
	}	// namespace glean
}    // namespace daw


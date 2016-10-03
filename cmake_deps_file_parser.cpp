// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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

#include <daw/daw_parser_helper.h>
#include <daw/daw_parser_addons.h>

#include "cmake_deps_impl.h"
#include "cmake_deps_file.h"
#include "cmake_deps_file_parser.h"

namespace daw {
	namespace cmake_deps {
		namespace {
			auto split( std::string const & str, std::string const & on ) {
				std::vector<size_t> results;
				size_t pos = 0;
				while( (pos = str.find_first_of( on, pos )) != std::string::npos ) {
					results.push_back( pos );
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
				throw cmake_deps_exception( "Error on line, no '=' symbol: " + line );
			}
			auto quotes = split( line.substr( pos ) , "\"" );
			if( quotes.size( ) != 2 ) {
				throw cmake_deps_exception( "Error on line, value isn't quoted properly" + line );
			}
			std::string value;
			if( quotes[1] - quotes[0] > 1 ) {
				value = line.substr( quotes[0] + 1, quotes[1] - 1 );
			}
			if( pos == 0 ) {
				throw cmake_deps_exception( "Error on line, empty key name" + line );
			}
			auto key = boost::trim_copy( line.substr( pos, quotes[0] - pos ) );
			if( key.empty() ) {
				throw cmake_deps_exception( "Error on line, empty key name" + line );
			}
			return std::make_pair( key, value );
		}

		cmake_deps_file parse_cmakes_deps( std::string const & deps_file ) {
			cmake_deps_file result;
			auto const lines = split( deps_file, "\n" );
			auto last_pos = 0;
			bool in_item = false;
			cmake_deps_item cur_item;
			for( auto pos : lines ) {
				auto const current_line =  boost::trim_copy( deps_file.substr( last_pos, pos - last_pos ) );
				last_pos = pos;
				auto const kv = parse_line( current_line );
				if( kv.first == "project_name" ) {
					if( in_item ) {
						result.dependencies.push_back( cur_item );
					}
					in_item = true;
					cur_item = cmake_deps_item{ };
					cur_item.project_name = kv.second;
				} else if( in_item ) {
					if( "uri" == kv.first ) {
						cur_item.uri = kv.second;
					} else if( "branch" == kv.first ) {
						cur_item.branch = kv.second;
					} else if( "decompress_command" == kv.first ) {
						cur_item.decompress_command = kv.second;
					} else if( "build_command" == kv.first ) {
						cur_item.build_command = kv.second;
					} else if( "install_command" == kv.first ) {
						cur_item.install_command = kv.second;
					} else {
						throw cmake_deps_exception( "Unknown key: " + current_line );
					}
				} else {
					throw cmake_deps_exception( "Error in dependencies file key specified without a project_name line first: " + current_line );
				}
			}
			if( in_item && !cur_item.project_name.empty( ) ) {
				result.dependencies.push_back( cur_item );
			}
			return result;
		}

		cmake_deps_file parse_cmakes_deps( boost::filesystem::path const & deps_file ) {
			assert( exists( deps_file ) && is_regular_file( deps_file ) );
			std::ifstream in_file;
			in_file.open( deps_file.native( ) );
			if( !in_file ) {
				std::stringstream ss;
				ss << "Could not open dependency file (" << deps_file << ")";
				throw cmake_deps_exception( ss.str( ) ); 
			}
			std::string in_str;
			std::copy( std::istream_iterator<char>( in_file ), std::istream_iterator<char>( ), std::back_inserter( in_str ) );
			in_file.close( );
			return parse_cmakes_deps( in_str );
		}
	}	// namespace cmake_deps
}    // namespace daw


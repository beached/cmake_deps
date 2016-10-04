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
#include <sstream>

#include <daw/kv_file.h>

#include "config.h"
#include "glean_impl.h"

namespace daw {
	namespace glean {
		namespace {
			auto get_home( ) {
				auto home = std::getenv( "HOME" );
				if( !home ) {
					home = std::getenv( "USERPROFILE" );
					if( !home ) {
						throw std::runtime_error( "Could not determine home folder" );
					}
				}
				std::string result;
				result = home;
				return result;
			}
		}

		glean_config get_config( ) {
			auto env_var = std::getenv( "CMAKE_DEPS_CONFIG" );
			auto const config_file = [&]( ) -> boost::filesystem::path {
				if( env_var ) {
					return boost::filesystem::path{ env_var };
				} else {
					boost::filesystem::path result{ get_home( ) };
					result /= ".glean.config";
					return result;
				}
			}( );
			bool const is_new_file = exists( config_file );
			assert( !is_new_file || is_regular_file( config_file ) );
			if( is_new_file ) {
				glean_config result;
				daw::kv_file{ }.add( "cache_folder", result.cache_folder ).to_file( config_file.native( ) );

				return result;
			}
			return glean_config{ config_file.native( ) };
		}

		namespace {
			std::string cache_path_string( boost::filesystem::path file_path ) {
				if( !exists( file_path ) || !is_regular_file( file_path ) ) {
					std::stringstream ss;
					ss << "File does not exist or is not a regular file (" << file_path << ')';
					throw glean_exception( ss.str( ) );
				}
				daw::kv_file kv_pairs{ canonical( file_path ).native( ) };
				auto const pos = std::find_if( kv_pairs.begin( ), kv_pairs.end( ), []( auto const & kv ) {
					static const std::string s = "cache_folder";
					return kv.key == s;
				} );


				if( pos == kv_pairs.end( ) ) {
					throw glean_exception( "Config file does not have cache_path value" );
				}
				return pos->value;
			}
		}

		glean_config::glean_config( boost::filesystem::path file_path ):
				cache_folder{ cache_path_string( std::move( file_path ) ) } { }

		glean_config::glean_config( std::string CacheFolder ):
				cache_folder{ std::move( CacheFolder ) } { }

		glean_config::glean_config( ):
				glean_config{ get_home( ) + "/.glean_cache" } { }

		glean_config::~glean_config( ) { }


	}	// namespace glean
}    // namespace daw


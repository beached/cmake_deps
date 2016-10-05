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
#include <iostream>
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
			auto env_var = std::getenv( "GLEAN_CONFIG" );
			auto const config_file = [&]( ) -> boost::filesystem::path {
				if( env_var ) {
					return boost::filesystem::path{ env_var };
				} else {
					boost::filesystem::path result{ get_home( ) };
					result /= ".glean.config";
					return result;
				}
			}( );
			return glean_config{ config_file };
		}

		glean_config::glean_config( std::string CacheFolder, std::string cmake_binary_path ):
				cache_folder{ std::move( CacheFolder ) }, 
				cmake_binary{ std::move( cmake_binary_path ) } { }

		glean_config::glean_config( ):
				glean_config{ get_home( ), "cmake" } {
		
			cache_folder /= ".glean_cache";
		}

		glean_config::glean_config( boost::filesystem::path file_path ):
				glean_config{ } {
		
			if( exists( file_path ) ) {
				if( !is_regular_file( file_path ) ) {
					std::stringstream ss;
					ss << "The config file (" << file_path << ") is not a regular file";
					throw glean_exception( ss.str( ) );
				}
				for( auto const & kv: daw::kv_file{ file_path.native( ) } ) {
					if( "cache_folder" == kv.key ) {
						cache_folder = boost::filesystem::path{ kv.value };
					} else if( "cmake_binary" == kv.key ) {
						cmake_binary = kv.value;
					} else {
						std::cerr << "WARNING: Unknown key(" << kv.key << ") value in cmake config file (" << file_path << ")" << std::endl;
					}
				}
			} else {
				daw::kv_file kv;
				kv.add( "cache_folder", cache_folder.native( ) );
				kv.add( "cmake_binary", cmake_binary );
				kv.to_file( file_path.native( ) );
			}
		}

		glean_config::~glean_config( ) { }
	}	// namespace glean
}    // namespace daw


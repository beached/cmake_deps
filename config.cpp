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

#include "config.h"

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
			std::string config_file = env_var ? env_var : get_home( ) + "/.glean.config";
			daw::glean::glean_config result;
			bool is_new_file = false;
			try {
				result = daw::json::from_file<daw::glean::glean_config>( config_file, false );
			} catch( std::exception const & ) {
				is_new_file = true;
			}

			if( is_new_file ) {
				try {
					result.to_file( config_file );
				} catch( std::exception const & ex ) {
					std::cerr << "Error writing config file '" << config_file << "'" << std::endl;
					std::cerr << "Exception: " << ex.what( ) << std::endl;
				}
			}
			return result;
		}


		glean_config::glean_config( std::string CacheFolder ):
				daw::json::JsonLink<glean_config>{ },
				cache_folder{ std::move( CacheFolder ) } { }

		glean_config::glean_config( ):
				glean_config{ get_home( ) + "/.glean_cache" } { }

		glean_config::~glean_config( ) { }


	}	// namespace glean
}    // namespace daw


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

#include <iostream>
#include <sstream>

#include <daw/daw_exception.h>
#include <daw/daw_read_file.h>
#include <daw/daw_string_view.h>
#include <daw/temp_file.h>

#include "daw/glean/glean_config.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	fs::path get_home( ) {
		auto home = std::getenv( "HOME" );
		if( !home ) {
			home = std::getenv( "USERPROFILE" );
		}
		daw::exception::postcondition_check<std::runtime_error>(
		  home, "Could not determine home folder" );
		return home;
	}

	glean_config get_config( ) {
		auto config_file = []( ) -> fs::path {
			auto const env_var = std::getenv( "GLEAN_CONFIG" );
			if( env_var ) {
				return {env_var};
			} else {
				return get_home( ) / ".glean.config";
			}
		}( );
		if( exists( config_file ) ) {
			return daw::json::from_json<glean_config>(
			  daw::read_file( config_file.string( ) ) );
		} else {
			return {};
		}
	}

	namespace {
		/*
		size_t write_data( char *data, size_t size, size_t nmemb,
		                   void *writer_data_p ) {
			if( nullptr == writer_data_p ) {
				return 0;
			}
			auto &writer_data = *reinterpret_cast<std::string *>( writer_data_p );
			if( data && size * nmemb > 0 ) {
				writer_data.append( data, size * nmemb );
			}
			return size * nmemb;
		}

		std::string download_file2( daw::string_view file_url ) {
			std::string result;
			curl_t curl;

			curl_easy_setopt( curl, CURLOPT_URL, file_url.data( ) );
			curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_data );
			curl_easy_setopt( curl, CURLOPT_WRITEDATA, &result );

			// Follow redirects
			curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1L );
			// 20 seems to be a fair number, but still a WAG
			curl_easy_setopt( curl, CURLOPT_MAXREDIRS, 20L );

			curl_easy_setopt( curl, CURLOPT_HTTPGET, 1L );
			auto res = curl_easy_perform( curl );
			if( res != CURLE_OK ) {
				throw std::runtime_error( "Could not download file" );
			}
			return result;
		}
		 */
	} // namespace

	/*
	daw::unique_temp_file download_file( daw::string_view url ) {
		auto tmp_file = daw::unique_temp_file( );
		auto out_file = tmp_file.secure_create_stream( );

		daw::exception::daw_throw_on_false<std::runtime_error>(
		  out_file, "Could not open tmp file for writing" );

		*out_file << download_file2( url );
		out_file->close( );
		return tmp_file;
	}
	 */
} // namespace daw::glean

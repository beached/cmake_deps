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
			  daw::read_file( config_file.c_str( ) ) );
		} else {
			return {};
		}
	}
} // namespace daw::glean

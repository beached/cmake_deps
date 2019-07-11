// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <string>
#include <vector>

#include <daw/daw_string_fmt.h>

#include "daw/glean/cmake_helper.h"
#include "daw/glean/glean_options.h"
#include "daw/glean/proc.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	std::vector<std::string>
	cmake_action_configure::build_args( fs::path build_path,
	                                    daw::glean::build_types bt ) const {

		auto result = std::vector<std::string>( );
		result.push_back( daw::fmt_t( "-DCMAKE_INSTALL_PREFIX:PATH={0}" )(
		  ( install_prefix / to_string( bt ) ).string( ) ) );

		if( has_glean ) {
			result.push_back( daw::fmt_t( "-DGLEAN_INSTALL_ROOT={0}" )(
			  ( install_prefix / to_string( bt ) ).string( ) ) );
		}
		result.push_back( "-S" );
		result.push_back( source_path.string( ) );
		result.push_back( "-B" );
		result.push_back( ( build_path / to_string( bt ) ).string( ) );

		result.insert( result.cend( ), custom_arguments.cbegin( ),
		               custom_arguments.cend( ) );
		return result;
	}

	std::vector<std::string>
	cmake_action_build::build_args( fs::path build_path,
	                                daw::glean::build_types bt ) const {
		return {"--build", ( build_path / to_string( bt ) ).string( )};
	}

	std::vector<std::string>
	cmake_action_install::build_args( fs::path build_path,
	                                  daw::glean::build_types bt ) const {
		return {"--build", ( build_path / to_string( bt ) ).string( ), "--target",
		        "install"};
	}
} // namespace daw::glean

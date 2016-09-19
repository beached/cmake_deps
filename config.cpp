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

#include <iostream>

#include "config.h"

namespace daw {
	namespace cmake_deps {
		void cmake_deps_config::make_links( ) {
			link_array( "repositories", m_repositories );
			link_boolean( "require_ssl", m_require_ssl );
			link_string( "install_prefix", m_install_prefix );
		}

		cmake_deps_config::cmake_deps_config( std::vector<std::string> repositories, bool require_ssl, std::string install_prefix ):
				daw::json::JsonLink<cmake_deps_config>{ },
				m_repositories{ std::move( repositories ) },
				m_require_ssl{ require_ssl },
				m_install_prefix{ std::move( install_prefix ) } {

			make_links( );
		}

		cmake_deps_config::~cmake_deps_config( ) { }

		cmake_deps_config::cmake_deps_config( ):
				cmake_deps_config( std::vector<std::string>{ "https://blah.com/cmake_deps.json" }, true, "/usr/local" ) { }

	}	// namespace cmake_deps
}    // namespace daw


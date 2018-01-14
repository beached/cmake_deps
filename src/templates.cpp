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

#include <string>
#include <sstream>

#include "templates.h"

namespace daw {
	namespace glean {
		namespace impl {
			std::string get_git_template( ) {
				static std::string const result =
				  "cmake_minimum_required( VERSION 2.8.12 )\n"
				  "project( <%call args=\"project_name\"%>_parent )\n"
				  "include( ExternalProject )\n"
				  "externalproject_add(\n"
				  "	<%call args=\"project_name\"%>\n"
				  " GIT_REPOSITORY \"<%call args=\"git_repo\"%>\"\n"
				  " SOURCE_DIR \"<%call args=\"source_directory\"%>\"\n"
				  " GIT_TAG \"<%call args=\"git_tag\"%>\"\n"
				  " INSTALL_DIR \"<%call args=\"install_directory\"%>\"\n"
				  "	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<%call args=\"install_directory\"%>\n"
				  ")\n";
				return result;
			}
		}
	}
}    // namespace daw


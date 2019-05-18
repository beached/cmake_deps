// The MIT License (MIT)
//
// Copyright (c) 2016-2018 Darrell Wright
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

#pragma once

#include <daw/daw_string_view.h>

namespace daw::glean::impl {
	namespace {
		inline constexpr daw::string_view get_git_template =
		  R"(cmake_minimum_required( VERSION 3.0.2 )
project( <%call args="project_name"%>_parent )
include( ExternalProject )
externalproject_add(
	<%call args="project_name"%>
	GIT_REPOSITORY "<%call args="git_repo"%>"
	SOURCE_DIR "<%call args="source_directory"%>"
	GIT_TAG "<%call args="git_tag"%>"
	INSTALL_DIR "<%call args="install_directory"%>"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<%call args="install_directory"%>
))";
	}
} // namespace daw::glean::impl

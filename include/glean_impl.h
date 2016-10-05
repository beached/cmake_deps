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

#pragma once

#include <boost/utility/string_ref.hpp>
#include <boost/filesystem/path.hpp>
#include <exception>

#include "config.h"

namespace daw {
	namespace glean {
		struct glean_exception: public std::runtime_error {
			glean_exception( boost::string_ref msg );
			~glean_exception( );
			glean_exception( glean_exception const & ) = default;
			glean_exception( glean_exception && ) = default;
			glean_exception & operator=( glean_exception const & ) = default;
			glean_exception & operator=( glean_exception && ) = default;
		};	// glean_exception

		void process_file( boost::filesystem::path const & depend_file, boost::filesystem::path const & prefix, glean_config const & cfg );
	}	// namespace glean
}    // namespace daw


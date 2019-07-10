// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <boost/program_options/variables_map.hpp>
#include <iosfwd>

#include "utilities.h"

namespace daw::glean {
	enum class build_types : bool { release, debug };
	std::ostream &operator<<( std::ostream &os, build_types bt );
	std::istream &operator>>( std::istream &is, build_types &bt );
	std::string to_string( build_types bt );

	enum class output_types : bool { process, cmake };
	std::ostream &operator<<( std::ostream &os, output_types bt );
	std::istream &operator>>( std::istream &is, output_types &bt );
	std::string to_string( output_types bt );

	struct glean_options {
		boost::program_options::variables_map vm;
		glean_options( int argc, char **argv );

		daw::glean::fs::path install_prefix( ) const;
		daw::glean::fs::path glean_cache( ) const;
		daw::glean::build_types build_type( ) const;
		daw::glean::output_types output_type( ) const;
	};
} // namespace daw::glean

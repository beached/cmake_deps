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

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <iterator>

#include <daw/daw_benchmark.h>

#include "glean_config.h"

void download_test_001( ) {
	auto const path = daw::glean::download_file( "https://www.google.ca" );
	std::cout << "Downloaded to file " << path.string( ) << '\n';
	daw::expecting( exists( *path ) );
	auto in_file = std::ifstream( path.string( ), std::ios::binary );
	daw::expecting( in_file );
	auto result = std::string( );
	std::copy( std::istreambuf_iterator<char>( in_file ),
	           std::istreambuf_iterator<char>( ), std::back_inserter( result ) );
}

int main( ) {
	download_test_001( );
}

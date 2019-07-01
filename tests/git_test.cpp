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

#include <cstdlib>
#include <iostream>

#include "git_helper.h"

int main( int argc, char **argv ) {
	if( argc != 3 ) {
		std::cerr << "Error in args, " << argv[0] << " repos local_folder\n";
		exit( EXIT_FAILURE );
	}
	std::string remote_uri = argv[1];
	std::string local_path = argv[2];
	std::cout << "cloning '" << remote_uri << "' to '" << local_path << "'\n";

	auto gh = daw::git_helper( );
	auto result = gh.clone( remote_uri, local_path );
	if( result != 0 ) {
		git_error const *err = giterr_last( );
		std::cerr << "Error while cloning: " << err->message << '\n';
	}
	return result;
}

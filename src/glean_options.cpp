// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "glean_options.h"

namespace daw {
	namespace {
		void ensure_system( ) noexcept {
			if( system( nullptr ) == 0 ) {
				std::cerr << "Could not call system function" << std::endl;
				exit( EXIT_FAILURE );
			}
		}

		boost::program_options::variables_map get_vm( int argc, char **argv ) {
			boost::program_options::options_description desc{"Options"};
			desc.add_options( )( "help", "print option descriptions" )(
			  "prefix", boost::program_options::value<std::string>( ),
			  "installation prefix folder" )(
			  "deps_file",
			  boost::program_options::value<boost::filesystem::path>( )
			    ->default_value( "./glean.txt" ),
			  "dependencies file" );

			boost::program_options::variables_map vm{};
			try {
				boost::program_options::store(
				  boost::program_options::parse_command_line( argc, argv, desc ), vm );

				if( vm.count( "help" ) ) {
					std::cout << "Command line options\n" << desc << std::endl;
					exit( EXIT_SUCCESS );
				}
				boost::program_options::notify( vm );
			} catch( boost::program_options::error const &po_error ) {
				std::cerr << "ERROR: " << po_error.what( ) << '\n';
				std::cerr << desc << std::endl;
				exit( EXIT_FAILURE );
			}
			return vm;
		}
	} // namespace

	glean_options::glean_options( int argc, char **argv )
	  : vm{get_vm( argc, argv )} {
		ensure_system( );
	}

	boost::filesystem::path glean_options::deps_file( ) const {
		if( vm.count( "deps_file" ) != 1 ) {
			std::cerr << "Must specify 1 dependency file\n";
			exit( EXIT_FAILURE );
		}
		auto result = vm["deps_file"].as<boost::filesystem::path>( );
		if( !exists( result ) ) {
			std::cerr << "Dependency file (" << result << ") does not exist\n";
			exit( EXIT_FAILURE );
		} else if( !is_regular_file( result ) ) {
			std::cerr << "Dependency file (" << result << ") is not a regular file\n";
			exit( EXIT_FAILURE );
		}
		return result;
	}

	boost::filesystem::path glean_options::prefix( ) const {
		boost::filesystem::path result;
		if( vm.count( "prefix" ) ) {
			result = vm["prefix"].as<boost::filesystem::path>( );
		} else {
			result = deps_file( ).parent_path( ) /= "glean_files";
		}
		if( exists( result ) ) {
			if( !is_directory( result ) ) {
				std::cerr << "Prefix folder (" << result << ") is not a directory\n";
				exit( EXIT_FAILURE );
			}
		} else {
			boost::filesystem::create_directory( result );
		}
		return result;
	}
} // namespace daw

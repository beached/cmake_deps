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

#include <cstdlib>
#include <iostream>

#include <boost/program_options.hpp>

#include "glean_config.h"
#include "glean_options.h"
#include "utilities.h"

namespace daw {
	namespace {
		void ensure_system( ) noexcept {
			if( system( nullptr ) == 0 ) {
				std::cerr << "Could not call system function\n";
				exit( EXIT_FAILURE );
			}
		}

		boost::program_options::variables_map get_vm( int argc, char **argv ) {
			boost::program_options::options_description desc{"Options"};
			desc.add_options( )( "help", "print option descriptions" )(
			  "cache",
			  boost::program_options::value<glean::fs::path>( )->default_value(
			    glean::get_home( ) / ".glean_cache" ),
			  "installation prefix folder" )(
			  "prefix",
			  boost::program_options::value<glean::fs::path>( )->default_value(
			    glean::fs::current_path( ) / ".glean" ),
			  "prefix for install" );

			boost::program_options::variables_map vm{};
			try {
				boost::program_options::store(
				  boost::program_options::parse_command_line( argc, argv, desc ), vm );

				if( vm.count( "help" ) ) {
					std::cout << "Command line options\n" << desc << '\n';
					exit( EXIT_SUCCESS );
				}
				boost::program_options::notify( vm );
			} catch( boost::program_options::error const &po_error ) {
				std::cerr << "ERROR: " << po_error.what( ) << '\n';
				std::cerr << desc << '\n';
				exit( EXIT_FAILURE );
			}
			return vm;
		}
	} // namespace

	glean_options::glean_options( int argc, char **argv )
	  : vm( get_vm( argc, argv ) ) {

		ensure_system( );
	}

	namespace {
		template<typename VM>
		glean::fs::path process_path_opt( VM const &vm, std::string const &name ) {
			glean::fs::path result;
			if( vm.count( name.c_str( ) ) ) {
				result = vm[name.c_str( )].template as<glean::fs::path>( );
			} else {
				std::cerr << name << " folder (" << result << ") not specified\n";
				exit( EXIT_FAILURE );
			}
			if( exists( result ) ) {
				if( !is_directory( result ) ) {
					std::cerr << name << " folder (" << result
					          << ") is not a directory\n";
					exit( EXIT_FAILURE );
				}
			}
			return result;
		}
	} // namespace

	glean::fs::path glean_options::install_prefix( ) const {
		auto result = process_path_opt( vm, "prefix" );
		if( !is_directory( result ) ) {
			if( exists( result ) ) {
				glean::fs::remove( result );
			}
			glean::fs::create_directory( result );
		}
		return result;
	}

	glean::fs::path glean_options::glean_cache( ) const {
		auto result = process_path_opt( vm, "cache" );
		if( !is_directory( result ) ) {
			if( exists( result ) ) {
				glean::fs::remove( result );
			}
			glean::fs::create_directory( result );
		}
		return result;
	}
} // namespace daw

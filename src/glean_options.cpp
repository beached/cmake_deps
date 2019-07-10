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
#include <iosfwd>

#include "daw/glean/build_types.h"
#include "daw/glean/glean_config.h"
#include "daw/glean/glean_options.h"
#include "daw/glean/utilities.h"

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
			  "prefix for install" )(
			  "build_type",
			  boost::program_options::value<::daw::build_types>( )->default_value(
			    ::daw::build_types::release ),
			  "type of build" )(
			  "output_type",
			  boost::program_options::value<::daw::output_types>( )->default_value(
			    ::daw::output_types::process ),
			  "type of output" );

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

	daw::build_types glean_options::build_type( ) const {
		return vm["build_type"].template as<daw::build_types>( );
	}

	std::ostream &operator<<( std::ostream &os, build_types bt ) {
		switch( bt ) {
		case build_types::release:
			os << "release";
			break;
		case build_types::debug:
			os << "debug";
			break;
		}
		return os;
	}

	std::istream &operator>>( std::istream &is, build_types &bt ) {
		std::string tmp{};
		is >> tmp;
		if( !tmp.empty( ) and ( ( tmp[0] == 'd' ) or ( tmp[0] == 'D' ) ) ) {
			bt = build_types::debug;
		} else {
			bt = build_types::release;
		}
		return is;
	}

	std::string to_string( build_types bt ) {
		switch( bt ) {
		case build_types::release:
			return "release";
		case build_types::debug:
			return "debug";
		}
		std::abort( );
	}

	daw::output_types glean_options::output_type( ) const {
		return vm["output_type"].template as<daw::output_types>( );
	}

	std::ostream &operator<<( std::ostream &os, output_types bt ) {
		switch( bt ) {
		case output_types::process:
			os << "process";
			break;
		case output_types::cmake:
			os << "cmake";
			break;
		}
		return os;
	}

	std::istream &operator>>( std::istream &is, output_types &bt ) {
		std::string tmp{};
		is >> tmp;
		if( !tmp.empty( ) and ( ( tmp[0] == 'c' ) or ( tmp[0] == 'C' ) ) ) {
			bt = output_types::cmake;
		} else {
			bt = output_types::process;
		}
		return is;
	}

	std::string to_string( output_types bt ) {
		switch( bt ) {
		case output_types::process:
			return "process";
		case output_types::cmake:
			return "cmake";
		}
		std::abort( );
	}

} // namespace daw

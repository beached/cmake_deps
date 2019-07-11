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

#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "daw/glean/build_types.h"
#include "daw/glean/glean_config.h"
#include "daw/glean/glean_options.h"
#include "daw/glean/logging.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	namespace {
		boost::program_options::variables_map get_vm( int argc, char **argv ) {
			auto desc = boost::program_options::options_description( "Options" );

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
			  boost::program_options::value<::daw::glean::build_types>( )
			    ->default_value( ::daw::glean::build_types::release ),
			  "type of build" )(
			  "output_type",
			  boost::program_options::value<::daw::glean::output_types>( )
			    ->default_value( ::daw::glean::output_types::process ),
			  "type of output" )(
			  "cmake_arg",
			  boost::program_options::value<std::vector<std::string>>( )
			    ->multitoken( ),
			  "additional commandline arguments to pass to cmake(1 per cmake_arg)" );

			auto vm = boost::program_options::variables_map( );
			try {
				boost::program_options::store(
				  boost::program_options::parse_command_line( argc, argv, desc ), vm );

				if( vm.count( "help" ) ) {
					auto ss = std::stringstream( );
					ss << desc;
					log_message << "Command line options\n" << ss.str( ) << '\n';
					exit( EXIT_SUCCESS );
				}
				boost::program_options::notify( vm );
			} catch( boost::program_options::error const &po_error ) {
				log_error << "ERROR: " << po_error.what( ) << '\n';
				auto ss = std::stringstream( );
				ss << desc;
				log_error << ss.str( ) << '\n';
				exit( EXIT_FAILURE );
			}
			return vm;
		}

		template<typename VM>
		fs::path process_path_opt( VM const &vm, std::string const &name ) {
			fs::path result;
			if( vm.count( name.c_str( ) ) ) {
				result = vm[name.c_str( )].template as<glean::fs::path>( );
			} else {
				auto ss = std::stringstream( );
				ss << result;
				log_error << name << " folder (" << ss.str( ) << ") not specified\n";
				exit( EXIT_FAILURE );
			}
			if( exists( result ) ) {
				if( !is_directory( result ) ) {
					auto ss = std::stringstream( );
					ss << result;
					log_error << name << " folder (" << ss.str( )
					          << ") is not a directory\n";
					exit( EXIT_FAILURE );
				}
			}
			return result;
		}
	} // namespace

	glean_options::glean_options( int argc, char **argv ) {

		auto const vm = get_vm( argc, argv );

		install_prefix = process_path_opt( vm, "prefix" );
		if( !is_directory( install_prefix ) ) {
			if( exists( install_prefix ) ) {
				glean::fs::remove( install_prefix );
			}
			glean::fs::create_directory( install_prefix );
		}

		glean_cache = process_path_opt( vm, "cache" );
		if( !is_directory( glean_cache ) ) {
			if( exists( glean_cache ) ) {
				glean::fs::remove( glean_cache );
			}
			glean::fs::create_directory( glean_cache );
		}

		build_type = vm["build_type"].template as<daw::glean::build_types>( );
		output_type = vm["output_type"].template as<daw::glean::output_types>( );

		if( !vm["cmake_arg"].empty( ) ) {
			cmake_args = vm["cmake_arg"].template as<std::vector<std::string>>( );
		}
	}

	::daw::glean::logger const &operator<<( ::daw::glean::logger const &os,
	                                        build_types bt ) {
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
} // namespace daw::glean

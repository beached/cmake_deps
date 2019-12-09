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
#include <sstream>

#include <daw/daw_read_file.h>
#include <daw/json/daw_json_link.h>

#include "daw/glean/build_types.h"
#include "daw/glean/dependency_options.h"
#include "daw/glean/glean_config.h"
#include "daw/glean/glean_options.h"
#include "daw/glean/logging.h"
#include "daw/glean/utilities.h"

extern "C" char const GIT_VERSION[];

namespace daw::glean {
	namespace {
		boost::program_options::variables_map get_vm( int argc, char **argv ) {
			auto desc = boost::program_options::options_description( "Options" );

			desc.add_options( )( "help", "print option descriptions" )(
			  "version", "print version information" )(
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
			  "type of build(release, debug)" )(
			  "output_type",
			  boost::program_options::value<::daw::glean::output_types>( )
			    ->default_value( ::daw::glean::output_types::process ),
			  "type of output" )( "dep_opts_file",
			                      boost::program_options::value<glean::fs::path>( ),
			                      "provide a dependency options override file" )(
			  "cmake_arg",
			  boost::program_options::value<std::vector<std::string>>( )
			    ->multitoken( ),
			  "additional commandline arguments to pass to cmake(1 per "
			  "cmake_arg)" )(
			  "jobs", boost::program_options::value<uint32_t>( )->default_value( 2U ),
			  "number of build jobs to run, if supported by build ssytem" )(
			  "use_first_dependency",
			  boost::program_options::value<bool>( )->default_value( false ),
			  "use the first dependency that provides a resource" );

			auto vm = boost::program_options::variables_map( );
			try {
				boost::program_options::store(
				  boost::program_options::parse_command_line( argc, argv, desc ), vm );

				if( vm.count( "help" ) ) {
					log_message << "glean - git tag/commit: " << GIT_VERSION << '\n';
					auto ss = std::stringstream( );
					ss << desc;
					log_message << "Command line options\n" << ss.str( ) << '\n';
					exit( EXIT_SUCCESS );
				}
				if( vm.count( "version" ) ) {
					log_message
					  << "glean by Darrell Wright\nhttps://github.com/beached/glean"
					  << '\n';
					log_message << "build from revision: " << GIT_VERSION << '\n';
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
			if( vm.count( name ) ) {
				result = vm[name].template as<glean::fs::path>( );
			} else {
				auto ss = std::stringstream( );
				ss << result;
				log_error << name << " folder (" << ss.str( ) << ") not specified\n";
				exit( EXIT_FAILURE );
			}
			if( exists( result ) ) {
				if( not is_directory( result ) ) {
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
		if( not is_directory( install_prefix ) ) {
			if( exists( install_prefix ) ) {
				glean::fs::remove( install_prefix );
			}
			glean::fs::create_directory( install_prefix );
		}

		glean_cache = process_path_opt( vm, "cache" );
		if( not is_directory( glean_cache ) ) {
			if( exists( glean_cache ) ) {
				glean::fs::remove( glean_cache );
			}
			glean::fs::create_directory( glean_cache );
		}

		build_type = vm["build_type"].template as<daw::glean::build_types>( );
		output_type = vm["output_type"].template as<daw::glean::output_types>( );
		use_first = vm["use_first_dependency"].template as<bool>( );
		jobs = vm["jobs"].template as<uint32_t>( );
		if( not vm["cmake_arg"].empty( ) ) {
			cmake_args = vm["cmake_arg"].template as<std::vector<std::string>>( );
		}

		if( not vm["dep_opts_file"].empty( ) ) {
			auto fname = vm["dep_opts_file"].template as<fs::path>( ).c_str( );
			auto str = daw::read_file( fname );
			if( not str ) {
				log_error << "Could not open dependency options file '" << fname
				          << "'\n";
				exit( EXIT_FAILURE );
			}
			dep_opts = daw::json::from_json<dependency_options>( str.value( ) );
		}
	}

	::daw::glean::logger const &operator<<( ::daw::glean::logger const &os,
	                                        build_types bt ) {
		switch( bt ) {
		case build_types::all:
			return os << "all";
		case build_types::release:
			return os << "release";
		case build_types::debug:
			return os << "debug";
		}
		std::abort( );
	}

	std::ostream &operator<<( std::ostream &os, build_types bt ) {
		switch( bt ) {
		case build_types::all:
			return os << "all";
		case build_types::release:
			return os << "release";
		case build_types::debug:
			return os << "debug";
		}
		std::abort( );
	}

	std::istream &operator>>( std::istream &is, build_types &bt ) {
		std::string tmp{};
		is >> tmp;
		if( tmp.empty( ) ) {
			throw std::runtime_error( "Empty build type" );
		}
		if( ( tmp[0] == 'd' ) or ( tmp[0] == 'D' ) ) {
			bt = build_types::debug;
		} else if( tmp[0] == 'a' or tmp[0] == 'A' ) {
			bt = build_types::all;
		} else if( tmp[0] == 'r' or tmp[0] == 'R' ) {
			bt = build_types::release;
		} else {
			throw std::runtime_error( "Unknown build type" );
		}
		return is;
	}

	std::string to_string( build_types bt ) {
		switch( bt ) {
		case build_types::release:
			return "release";
		case build_types::debug:
			return "debug";
		case build_types::all:
			return "all";
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
		if( not tmp.empty( ) and ( ( tmp[0] == 'c' ) or ( tmp[0] == 'C' ) ) ) {
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

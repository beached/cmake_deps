
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

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <string>
#include <iostream>

#include "config.h"

auto get_home( ) {
	auto home = std::getenv( "HOME" );
	if( !home ) {
		home = std::getenv( "USERPROFILE" );
		if( !home ) {
			throw std::runtime_error( "Could not determine home folder" );
		}
	}
	std::string result;
	result = home;
	return result;
}

auto get_config( ) {
	auto env_var = std::getenv( "CMAKE_DEPS_CONFIG" );
	std::string config_file = env_var ? env_var : get_home( ) + "/.cmake_deps.config";
	daw::cmake_deps::cmake_deps_config result;
	bool is_new_file = false;
	try {
		result = daw::json::from_file<daw::cmake_deps::cmake_deps_config>( config_file, false );
	} catch( std::exception const & ) {
		is_new_file = true;
	}

	if( is_new_file ) {
		try {
			result.to_file( config_file );
		} catch( std::exception const & ex ) {
			std::cerr << "Error writing config file '" << config_file << "'" << std::endl;
			std::cerr << "Exception: " << ex.what( ) << std::endl;
		}
	}
	return result;
}

int main( int argc, char** argv ) {
	auto config = get_config( );
	
	boost::program_options::options_description desc{ "Options" };
	desc.add_options( )
		( "help", "print option descriptions" )
		( "prefix", boost::program_options::value<boost::filesystem::path>( )->default_value( config.install_prefix ), "installation prefix folder" )
		( "cache_folder", boost::program_options::value<boost::filesystem::path>( )->default_value( config.cache_folder ), "installation cache folder" )
		( "deps_file", boost::program_options::value<boost::filesystem::path>( )->default_value( "./cmake_deps.config" ), "dependencies file" );

	boost::program_options::variables_map vm;
	try {
		boost::program_options::store( boost::program_options::parse_command_line( argc, argv, desc ), vm );
		
		if( vm.count( "help" ) ) {
			std::cout << "Command line options\n" << desc << std::endl;
			return EXIT_SUCCESS;
		}
		boost::program_options::notify( vm );
	} catch( boost::program_options::error const & po_error ) {
		std::cerr << "ERROR: " << po_error.what( ) << '\n';
		std::cerr << desc << std::endl;
		return EXIT_FAILURE;
	}
	auto prefix = vm[ "prefix" ].as<boost::filesystem::path>( ); 
	auto deps_file = vm[ "deps_file" ].as<boost::filesystem::path>( );

	if( !exists( deps_file ) ) {
		std::cerr << "Dependency file (" << deps_file << ") does not exist\n";
		return EXIT_FAILURE;
	} else if( !exists( prefix ) || !is_directory( prefix ) ) {
		std::cerr << "Prefix folder (" << prefix << ") must exist and be a directory\n";
		return EXIT_FAILURE;
	}
	std::cout << (config.to_string( )) << std::endl;

	return EXIT_SUCCESS;
}

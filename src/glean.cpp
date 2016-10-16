
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
#include <iostream>
#include <glean_impl.h>

#include "glean_file.h"
#include "utilities.h"

int main( int argc, char** argv ) {
	auto config = daw::glean::get_config( );
	if( !exists( config.cache_folder ) ) {
		create_directory( config.cache_folder );
	}
	if( !exists( config.cache_folder ) || !is_directory( config.cache_folder ) ) {
		std::stringstream ss;
		ss << "Cache root (" << config.cache_folder << ") does not exist or is not a directory";
		throw daw::glean::glean_exception( ss.str( ) );
	}
	boost::program_options::options_description desc{ "Options" };
	desc.add_options( )
		( "help", "print option descriptions" )
		( "prefix", boost::program_options::value<std::string>( ), "installation prefix folder" )
		( "deps_file", boost::program_options::value<boost::filesystem::path>( )->default_value( "./glean.txt" ), "dependencies file" );

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

	auto deps_file = vm["deps_file"].as<boost::filesystem::path>( );
	
	if( !exists( deps_file ) ) {
		std::cerr << "Dependency file (" << deps_file << ") does not exist\n";
		return EXIT_FAILURE;
	} else if( !is_regular_file( deps_file ) ) {
		std::cerr << "Dependency file (" << deps_file << ") is not a regular file\n";
		return EXIT_FAILURE;
	}

	auto prefix = [&]( ) -> boost::filesystem::path {		
		if( vm.count( "prefix" ) ) {
			return vm["prefix"].as<boost::filesystem::path>( ); 
		} else {
			return deps_file.parent_path( ) /= "glean_files";
		}
	}( );

	if( exists( prefix ) ) {
		if( !is_directory( prefix ) ) {
			std::cerr << "Prefix folder (" << prefix << ") is not a directory\n";
			return EXIT_FAILURE;
		}
	} else {
		boost::filesystem::create_directory( prefix );
	}
	daw::glean::process_file( deps_file, prefix, config );

	return EXIT_SUCCESS;
}

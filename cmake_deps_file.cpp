
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

#include <string>
#include <vector>

#include <daw/json/daw_json_link.h>

#include "cmake_deps_file.h"

namespace daw {
	namespace cmake_deps {
		std::string to_string( item_types t ) {
			static std::array<std::string, 3> const result = { { "none", "git", "custom" } };
			return result[static_cast<uint8_t>( t )];
		}

		std::ostream & operator<<( std::ostream & os, item_types const & item ) {
			return (os << to_string( item ));
		}

		std::istream & operator>>( std::istream & is, item_types & item ) {
			static std::unordered_map<std::string, item_types> const result = { { "none", item_types::none }, { "git", item_types::git }, { "custom", item_types::custom } };
			std::string tmp;
			is >> tmp;
			item = result.at( tmp );
			return is;
		}

		
		cmake_deps_item::cmake_deps_item( item_types Type, boost::optional<std::string> Uri, boost::optional<std::string> Branch, boost::optional<std::string> DecompressCommand, boost::optional<std::string> BuildCommand, boost::optional<std::string> InstallCommand ):
				type{ Type },
				uri( std::move( Uri ) ),
				branch( std::move( Branch ) ),
				decompress_command( std::move( DecompressCommand ) ),
				build_command( std::move( BuildCommand ) ),
				install_command( std::move( InstallCommand ) ) {
	
			link_streamable( "type", type );
			link_string( "uri", uri );
			link_string( "branch", branch );
			link_string( "decompress_command", decompress_command );
			link_string( "build_command", build_command );
			link_string( "install_command", install_command );
		}

		cmake_deps_item::cmake_deps_item( ):
				cmake_deps_item( item_types::none ) { }

		cmake_deps_item::~cmake_deps_item( ) { }

		cmake_deps_file::cmake_deps_file( ):
				daw::json::JsonLink<cmake_deps_file>{ },
				dependencies{ } {

			link_array( "dependencies", dependencies );
		}

		cmake_deps_file::~cmake_deps_file( ) { }

	}	// namespace cmake_deps
}    // namespace daw


// The MIT License (MIT)
//
// Copyright (c) 2016-2018 Darrell Wright
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

#include <array>
#include <string>
#include <unordered_map>

#include "glean_file.h"

namespace daw {
	namespace glean {
		std::string to_string( download_type_t t ) {
			static std::array<std::string, 4> const result = {{"none", "git", "uri", "github"}};
			return result[static_cast<uint8_t>( t )];
		}

		download_type_t download_type_from_string( std::string const & str ) {
			static std::unordered_map<std::string, download_type_t> const result = {{"none", download_type_t::none},
			                                                                        {"git", download_type_t::git},
			                                                                        {"uri", download_type_t::uri},
			                                                                        {"github", download_type_t::github}};
			return result.at( str );
		}

		std::ostream &operator<<( std::ostream &os, download_type_t const &item ) {
			return ( os << to_string( item ) );
		}

		glean_item::glean_item( download_type_t Type, std::string ProjectName, boost::optional<std::string> Uri,
		                        boost::optional<std::string> Branch, boost::optional<std::string> DecompressCommand,
		                        boost::optional<std::string> BuildCommand, boost::optional<std::string> InstallCommand )
		  : type{Type}
		  , project_name{std::move( ProjectName )}
		  , uri( std::move( Uri ) )
		  , branch( std::move( Branch ) )
		  , decompress_command( std::move( DecompressCommand ) )
		  , build_command( std::move( BuildCommand ) )
		  , install_command( std::move( InstallCommand ) ) {}

		glean_item::~glean_item( ) {}

		glean_file::glean_file( )
		  : dependencies{} {}

		glean_file::~glean_file( ) {}

	} // namespace glean
} // namespace daw

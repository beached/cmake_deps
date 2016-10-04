
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

#pragma once

#include <boost/optional.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace daw {
	namespace glean {
		enum class item_types: uint8_t { none, git, custom };
		std::string to_string( item_types t );
		item_types items_type_from_string( std::string const & str );

		std::ostream & operator<<( std::ostream & os, item_types const & item );
		std::istream & operator>>( std::istream & is, item_types & item );

		struct glean_item {
			item_types type;
			std::string project_name;
			boost::optional<std::string> uri;
			boost::optional<std::string> branch;
			boost::optional<std::string> decompress_command;
			boost::optional<std::string> build_command;
			boost::optional<std::string> install_command;
			using str_opt = boost::optional<std::string>;
			glean_item( item_types Type,
							 std::string ProjectName = "",
							 boost::optional<std::string> Uri = str_opt{ },
							 boost::optional<std::string> Branch = str_opt{ },
							 boost::optional<std::string> DecompressCommand = str_opt{ },
							 boost::optional<std::string> BuildCommand = str_opt{ },
							 boost::optional<std::string> InstallCommand = str_opt{ } );
			glean_item( );
			~glean_item( );

			glean_item( glean_item const & ) = default;
			glean_item( glean_item && ) = default;
			glean_item & operator=( glean_item const & ) = default;
			glean_item & operator=( glean_item && ) = default;
		};	// glean_item

		struct glean_file {
			std::vector<glean_item> dependencies;

			glean_file( );
			~glean_file( );

			glean_file( glean_file const & ) = default;
			glean_file( glean_file && ) = default;
			glean_file & operator=( glean_file const & ) = default;
			glean_file & operator=( glean_file && ) = default;
		};	// glean_file

	}	// namespace glean
}    // namespace daw


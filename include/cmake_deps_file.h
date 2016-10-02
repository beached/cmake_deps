
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

#include <iostream>
#include <string>
#include <vector>

#include <daw/json/daw_json_link.h>
#include <daw/daw_optional.h>

namespace daw {
	namespace cmake_deps {
		enum class item_types: uint8_t { none, git, custom };
		std::string to_string( item_types t );
		std::ostream & operator<<( std::ostream & os, item_types const & item );
		std::istream & operator>>( std::istream & is, item_types & item );

		struct cmake_deps_item: public daw::json::JsonLink<cmake_deps_item> {
			item_types type;
			std::string uri;
			daw::optional<std::string> branch;
			daw::optional<std::string> decompress_command;
			daw::optional<std::string> build_command;
			daw::optional<std::string> install_command;
			using str_opt = daw::optional<std::string>;
			cmake_deps_item( item_types Type, daw::optional<std::string> Uri = str_opt{ }, daw::optional<std::string> Branch = str_opt{ }, daw::optional<std::string> DecompressCommand = str_opt{ }, daw::optional<std::string> BuildCommand = str_opt{ }, daw::optional<std::string> InstallCommand = str_opt{ } );
			cmake_deps_item( );
			~cmake_deps_item( );

			cmake_deps_item( cmake_deps_item const & ) = default;
			cmake_deps_item( cmake_deps_item && ) = default;
			cmake_deps_item & operator=( cmake_deps_item const & ) = default;
			cmake_deps_item & operator=( cmake_deps_item && ) = default;
		};	// cmake_deps_item

		struct cmake_deps_file: public daw::json::JsonLink<cmake_deps_file> {
			std::vector<cmake_deps_item> dependencies;

			cmake_deps_file( );
			~cmake_deps_file( );

			cmake_deps_file( cmake_deps_file const & ) = default;
			cmake_deps_file( cmake_deps_file && ) = default;
			cmake_deps_file & operator=( cmake_deps_file const & ) = default;
			cmake_deps_file & operator=( cmake_deps_file && ) = default;
		};	// cmake_deps_file

	}	// namespace cmake_deps
}    // namespace daw


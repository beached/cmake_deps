
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

#pragma once

#include <boost/optional.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace daw {
	namespace glean {
		struct git_download_t {
			std::string reposistory_uri;
			boost::optional<std::string> repository_tag;
		};

		struct uri_download_t {
			std::string uri;
		};

		struct github_download_t {
			std::string reposistory;
			boost::optional<std::string> repository_tag;
		};

		enum class download_type_t : uint8_t { none, git, uri, github };
		std::string to_string( download_type_t t );
		download_type_t download_type_from_string( std::string const & str );

		std::ostream &operator<<( std::ostream &os, download_type_t const &item );
		std::istream &operator>>( std::istream &is, download_type_t &item );

		struct cmake_build_t {
			std::string arguments;
		};

		enum class build_type_t: uint8_t { none, cmake };


		struct glean_item {
			download_type_t type;
			std::string project_name;
			boost::optional<std::string> uri;
			boost::optional<std::string> branch;
			boost::optional<std::string> decompress_command;
			boost::optional<std::string> build_command;
			boost::optional<std::string> install_command;
			using str_opt = boost::optional<std::string>;

			explicit glean_item( download_type_t Type = download_type_t::none, std::string ProjectName = "",
			                     boost::optional<std::string> Uri = str_opt{},
			                     boost::optional<std::string> Branch = str_opt{},
			                     boost::optional<std::string> DecompressCommand = str_opt{},
			                     boost::optional<std::string> BuildCommand = str_opt{},
			                     boost::optional<std::string> InstallCommand = str_opt{} );

			~glean_item( );

			glean_item( glean_item const & ) = default;
			glean_item( glean_item && ) noexcept = default;
			glean_item &operator=( glean_item const & ) = default;
			glean_item &operator=( glean_item && ) noexcept = default;
		}; // glean_item

		struct glean_file {
			std::vector<glean_item> dependencies;

			glean_file( );
			~glean_file( );

			glean_file( glean_file const & ) = default;
			glean_file( glean_file && ) noexcept = default;
			glean_file &operator=( glean_file const & ) = default;
			glean_file &operator=( glean_file && ) noexcept = default;
		}; // glean_file

	} // namespace glean
} // namespace daw

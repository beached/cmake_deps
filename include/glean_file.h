// The MIT License (MIT)
//
// Copyright (c) 2016-2019 Darrell Wright
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

#pragma once

#include <iosfwd>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#include <daw/json/daw_json_link.h>

#include "build_types.h"
#include "dependency.h"
#include "download_types.h"

namespace daw::glean {

	// Data structure to represent the dependencies
	struct glean_file_item {
		std::string name;
		std::string download_type;
		std::string build_type;
		std::string uri;
		std::optional<std::string> version;
	};

	inline auto describe_json_class( glean_file_item ) {
		using namespace daw::json;
		static constexpr char const name[] = "name";
		static constexpr char const download_type[] = "download_type";
		static constexpr char const build_type[] = "build_type";
		static constexpr char const uri[] = "uri";
		static constexpr char const version[] = "version";
		return class_description_t<json_string<name>, json_string<download_type>,
		                           json_string<build_type>, json_string<uri>,
		                           json_nullable<json_string<version>>>{};
	}

	struct glean_config_file {
		std::string provides;
		std::vector<glean_file_item> dependencies;
	};

	inline auto describe_json_class( glean_config_file ) {
		using namespace daw::json;
		static constexpr char const provides[] = "provides";
		static constexpr char const dependencies[] = "dependencies";
		return class_description_t<
		  json_string<provides>,
		  json_array<no_name, std::vector<glean_file_item>,
		             json_class<dependencies, glean_file_item>>>{};
	}

	// Old config file items
	/*
	std::unordered_set<dependency>
	process_config_file( std::string config_file_path, std::string prefix );

	struct git_download_t {
	  std::string reposistory_uri{};
	  std::optional<std::string> repository_tag{};
	};

	struct uri_download_t {
	  std::string uri{};
	};

	struct github_download_t {
	  std::string reposistory{};
	  std::optional<std::string> repository_tag{};
	};

	enum class download_type_t : uint8_t { none, git, uri, github };
	std::string to_string( download_type_t t );
	download_type_t download_type_from_string( std::string const &str );

	std::ostream &operator<<( std::ostream &os, download_type_t const &item );
	std::istream &operator>>( std::istream &is, download_type_t &item );

	struct cmake_build_t {
	  std::string arguments{};
	};

	enum class build_type_t : uint8_t { none, cmake };

	struct glean_item {
	  download_type_t type = download_type_t::none;
	  std::string project_name{};

	  std::optional<std::string> uri{};
	  std::optional<std::string> branch{};
	  std::optional<std::string> decompress_command{};
	  std::optional<std::string> build_command{};
	  std::optional<std::string> install_command{};

	  glean_item( ) = default;

	  explicit glean_item( download_type_t Type, std::string ProjectName = "",
	                       std::optional<std::string> Uri = {},
	                       std::optional<std::string> Branch = {},
	                       std::optional<std::string> DecompressCommand = {},
	                       std::optional<std::string> BuildCommand = {},
	                       std::optional<std::string> InstallCommand = {} );

	}; // glean_item

	struct glean_file {
	  std::vector<glean_item> dependencies{};

	  glean_file( ) = default;
	}; // glean_file
	 */
} // namespace daw::glean

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
#include <variant>
#include <vector>

#include <daw/daw_graph.h>
#include <daw/json/daw_json_link.h>

#include "build_types.h"
#include "dependency.h"
#include "download_types.h"
#include "glean_options.h"

namespace daw::glean {

	// Data structure to represent the dependencies
	struct glean_file_item {
		std::string name;
		std::string download_type;
		std::string build_type;
		std::string uri;
		std::optional<std::string> version;
		std::optional<std::string> custom_options;
	};

	static constexpr char const glean_file_item_name[] = "name";
	static constexpr char const glean_file_item_download_type[] = "download_type";
	static constexpr char const glean_file_item_build_type[] = "build_type";
	static constexpr char const glean_file_item_uri[] = "uri";
	static constexpr char const glean_file_item_version[] = "version";
	static constexpr char const glean_file_item_custom_options[] =
	  "custom_options";

	inline auto describe_json_class( glean_file_item ) {
		using namespace daw::json;

		return class_description_t<
		  json_string<glean_file_item_name>,
		  json_string<glean_file_item_download_type>,
		  json_string<glean_file_item_build_type>, json_string<glean_file_item_uri>,
		  json_nullable<json_string<glean_file_item_version>>,
		  json_nullable<json_string<glean_file_item_custom_options>>>{};
	}

	struct glean_config_file {
		std::string provides;
		std::string build_type;
		std::vector<glean_file_item> dependencies;
	};

	static constexpr char const glean_config_file_provides[] = "provides";
	static constexpr char const glean_config_file_build_type[] = "build_type";
	static constexpr char const glean_config_file_dependencies[] = "dependencies";

	inline auto describe_json_class( glean_config_file ) {
		using namespace daw::json;

		return class_description_t<
		  json_string<glean_config_file_provides>,
		  json_string<glean_config_file_build_type>,
		  json_array<glean_config_file_dependencies, std::vector<glean_file_item>,
		             json_class<no_name, glean_file_item>>>{};
	}

	void process_config_file( fs::path const &config_file_path,
	                          glean_options const &opts );
} // namespace daw::glean

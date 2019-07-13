// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <string>
#include <vector>

#include <daw/json/daw_json_link.h>

namespace daw::glean {
	// Data structure to represent the dependencies
	struct glean_file_item {
		std::string provides{};
		std::string download_type{};
		std::string build_type{};
		std::string uri{};
		std::string version{};
		std::string custom_options{};
		std::vector<std::string> cmake_args{};
	};

	namespace symbols_glean_file_item {
		static constexpr char const provides[] = "provides";
		static constexpr char const download_type[] = "download_type";
		static constexpr char const build_type[] = "build_type";
		static constexpr char const uri[] = "uri";
		static constexpr char const version[] = "version";
		static constexpr char const custom_options[] = "custom_options";
		static constexpr char const cmake_args[] = "cmake_args";
	} // namespace symbols_glean_file_item

	inline auto describe_json_class( glean_file_item ) {
		using namespace daw::json;

		return class_description_t<
		  json_string<symbols_glean_file_item::provides>,
		  json_string<symbols_glean_file_item::download_type>,
		  json_string<symbols_glean_file_item::build_type>,
		  json_string<symbols_glean_file_item::uri>,
		  json_nullable<json_string<symbols_glean_file_item::version>>,
		  json_nullable<json_string<symbols_glean_file_item::custom_options>>,
		  json_nullable<
		    json_array<symbols_glean_file_item::cmake_args,
		               std::vector<std::string>, json_string<no_name>>>>{};
	}

	struct glean_config_file {
		std::string provides{};
		std::string build_type{};
		std::vector<glean_file_item> dependencies{};
	};

	namespace symbols_glean_config_file {
		static constexpr char const provides[] = "provides";
		static constexpr char const build_type[] = "build_type";
		static constexpr char const dependencies[] = "dependencies";
	} // namespace symbols_glean_config_file

	inline auto describe_json_class( glean_config_file ) {
		using namespace daw::json;

		return class_description_t<
		  json_string<symbols_glean_config_file::provides>,
		  json_string<symbols_glean_config_file::build_type>,
		  json_array<symbols_glean_config_file::dependencies,
		             std::vector<glean_file_item>,
		             json_class<no_name, glean_file_item>>>{};
	}
} // namespace daw::glean

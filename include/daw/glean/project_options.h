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

namespace daw {
	enum class dependency_merge_type : bool { append, replace };

	constexpr dependency_merge_type_t
	from_string( daw::tag_t<dependency_merge_type_t>,
	             std::string_view sv ) noexcept {
		constexpr auto const values =
		  daw::make_bounded_hash_map<std::string_view, dependency_merge_type>(
		    {"append", dependency_merge_type::append},
		    {"replace", dependency_merge_type::replace} );

		return values[sv];
	}

	constexpr std::string_view
	to_string( dependency_merge_type_t merge_type ) noexcept {
		constexpr auto const values =
		  daw::make_bounded_hash_map<std::string_view, dependency_merge_type>(
		    {dependency_merge_type::append, "append"},
		    {dependency_merge_type::replace, "replace"} );

		return values[merge_type];
	}

	struct dependency_option {
		std::string dep_name;
		std::string opt_name;
		std::string opt_value;
		dependency_merge_type merge_type;
	};

	namespace symbols_dependency_option {
		static constexpr char const dep_name[] = "dep_name";
		static constexpr char const opt_name[] = "opt_name";
		static constexpr char const opt_value[] = "opt_value";
		static constexpr char const merge_type[] = "merge_type";
	} // namespace symbols_dependency_option

	inline auto describe_json_class( dependency_option ) {
		using namespace daw::json;
		return class_description_t<
		  json_string<symbols_dependency_option::dep_name>,
		  json_string<symbols_dependency_option::opt_name>,
		  json_string<symbols_dependency_option::opt_value>,
		  json_custom<symbols_dependency_option::merge_type,
		              dependency_merge_type>>{};
	}

	inline auto to_json_data( dependency_option const &dep_opt ) {
		return std::make_tuple( dep_opt.opt_name, dep_opt.opt_name,
		                        dep_opt.opt_value, dep_opt.merge_type );
	}

	struct dependency_options {
		std::vector<dependency_option> values;
	};

	namespace symbols_dependency_options {
		static constexpr char const values[] = "values";
	}

	inline auto describe_json_class( dependency_options ) {
		using namespace daw::json;
		return class_description_t<json_array<
		  symbols_dependency_options::values, std::vector<dependency_option>,
		  json_class<no_name, dependency_option>>>{};
	}

	inline auto to_json_data( dependency_options const &dep_opts ) {
		return std::make_tuple( dep_opts.values );
	}

} // namespace daw

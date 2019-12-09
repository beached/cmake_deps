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

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <daw/daw_optional.h>
#include <daw/json/daw_json_link.h>

namespace daw {
	enum class dependency_merge_type : uint8_t { append, replace };

	constexpr dependency_merge_type
	from_string( daw::tag_t<dependency_merge_type>,
	             std::string_view sv ) noexcept {
		if( sv == "replace" ) {
			return dependency_merge_type::replace;
		}
		return dependency_merge_type::append;
	}

	constexpr std::string_view
	to_string( dependency_merge_type merge_type ) noexcept {
		using namespace std::string_view_literals;
		constexpr std::string_view const values[] = {"append"sv, "replace"sv};

		return values[static_cast<size_t>( merge_type )];
	}

	enum class opt_names { cmake_args };

	constexpr opt_names from_string( daw::tag_t<opt_names>,
	                                 std::string_view sv ) noexcept {
		if( sv == "cmake_args" ) {
			return opt_names::cmake_args;
		}
		std::cerr << "Invalid dependency option name\n";
		exit( EXIT_FAILURE );
	}

	constexpr std::string_view to_string( opt_names ) noexcept {
		return "cmake_args";
	}

	struct dependency_option {
		std::string dep_name;
		opt_names opt_name;
		std::vector<std::string> opt_value;
		dependency_merge_type merge_type = dependency_merge_type::append;
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
		  json_custom<symbols_dependency_option::opt_name, opt_names>,
		  json_array<symbols_dependency_option::opt_value, std::string>,
		  json_custom<symbols_dependency_option::merge_type,
		              dependency_merge_type>>{};
	}

	inline auto to_json_data( dependency_option const &dep_opt ) {
		return std::make_tuple( dep_opt.opt_name, dep_opt.opt_name,
		                        dep_opt.opt_value, dep_opt.merge_type );
	}

	struct dependency_options {
		std::vector<dependency_option> values;

		daw::optional<dependency_option const &>
		get( std::string_view dep_name ) const {
			auto pos = std::find_if( values.begin( ), values.end( ),
			                         [dep_name]( dependency_option const &d ) {
				                         return d.dep_name == dep_name;
			                         } );
			if( pos != values.end( ) ) {
				return daw::optional<dependency_option const &>( *pos );
			}
			return {};
		}
	};

	namespace symbols_dependency_options {
		static constexpr char const values[] = "values";
	}

	inline auto describe_json_class( dependency_options ) {
		using namespace daw::json;
		return class_description_t<
		  json_array<symbols_dependency_options::values, dependency_option>>{};
	}

	inline auto to_json_data( dependency_options const &dep_opts ) {
		return std::make_tuple( dep_opts.values );
	}

} // namespace daw

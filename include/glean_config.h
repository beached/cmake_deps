
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

#include <daw/daw_string_view.h>
#include <daw/json/daw_json_link.h>
#include <daw/temp_file.h>

#include "utilities.h"

namespace daw::glean {
	fs::path get_home( );

	struct glean_config {
		fs::path cache_folder = fs::path( get_home( ) ) / ".glean_cache";
		fs::path cmake_binary = "cmake";
	}; // glean_config

	inline auto describe_json_class( glean_config ) {
		using namespace daw::json;
		static constexpr char const cache_folder[] = "cache_folder";
		static constexpr char const cmake_binary[] = "cmake_binary";
		return class_description_t<json_string<cache_folder>,
		                           json_string<cmake_binary>>{};
	}

	inline auto to_json_data( glean_config const &gc ) {
		return std::make_tuple( gc.cache_folder.string( ),
		                        gc.cmake_binary.string( ) );
	}

	glean_config get_config( );

	/// @brief Download a file to a temporary location
	/// @param url Location of file to download
	/// @return path to downloaded file
	/// @post The file exists and will be deleted upon program termination
	daw::unique_temp_file download_file( daw::string_view url );
} // namespace daw::glean

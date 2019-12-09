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

#include <daw/daw_string_view.h>

#include "action_status.h"
#include "glean_options.h"
#include "utilities.h"

namespace daw::glean {
	struct glean_file_item;

	struct build_cmake {
		static constexpr daw::string_view type_id = "cmake";
		fs::path m_cache_path{};
		fs::path m_install_prefix{};
		glean_options const *m_opt = nullptr;
		bool m_has_glean;

		build_cmake( fs::path const &cache_path, fs::path const &install_prefix,
		             glean_options const &opts, bool has_glean ) noexcept;

		[[nodiscard]] action_status build( daw::glean::build_types bt,
		                                   glean_file_item const &file_dep ) const;
		[[nodiscard]] action_status install( daw::glean::build_types bt ) const;
	};

} // namespace daw::glean

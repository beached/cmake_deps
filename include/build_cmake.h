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
#include "utilities.h"

namespace daw::glean {
	struct build_cmake {
		static constexpr daw::string_view type_id = "cmake";
		fs::path m_source;
		fs::path m_build;
		fs::path m_install;

		inline build_cmake( fs::path source, fs::path build,
		                    fs::path install ) noexcept
		  : m_source( std::move( source ) )
		  , m_build( std::move( build ) )
		  , m_install( std::move( install ) ) {}

		inline build_cmake( std::string const &source, std::string const &build,
		                    std::string const &install )
		  : m_source( source )
		  , m_build( build )
		  , m_install( install ) {}

		inline action_status build( ) const {
			return action_status::failure;
		}

		inline action_status install( ) const {
			return action_status::failure;
		}
	};
} // namespace daw::glean

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

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "action_status.h"
#include "build_types.h"
#include "download_types.h"
#include "glean_file_item.h"
#include "glean_options.h"

namespace daw::glean {

	struct [[nodiscard]] dependency {
		struct [[nodiscard]] item_t {
			build_types_t build_type;
			std::optional<glean_file_item> file_dep{};
		};

	private:
		std::string m_name;
		std::vector<item_t> m_alternatives{};
		size_t m_index = 0;
		item_t const &alt( ) const;
		item_t &alt( );
		bool m_has_downloaded = false; // hack for now

	public:
		dependency( std::string name, build_types_t const &build_type );

		dependency( std::string name, build_types_t const &build_type,
		            glean_file_item const &file_dep );

		[[nodiscard]] std::string const &name( ) const noexcept;
		[[nodiscard]] action_status build( daw::glean::build_types bt ) const;
		[[nodiscard]] action_status install( daw::glean::build_types bt ) const;
		[[nodiscard]] glean_file_item const &file_dep( ) const noexcept;
		[[nodiscard]] bool has_file_dep( ) const noexcept;
		[[nodiscard]] std::vector<item_t> &alternatives( ) noexcept;
		[[nodiscard]] std::vector<item_t> const &alternatives( ) const noexcept;
		void add_alternative( glean_file_item const &gfi );

		[[nodiscard]] inline bool &has_downloaded( ) noexcept {
			return m_has_downloaded;
		}
		[[nodiscard]] inline bool const &has_downloaded( ) const noexcept {
			return m_has_downloaded;
		}
	};
} // namespace daw::glean

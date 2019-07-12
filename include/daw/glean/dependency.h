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
#include <utility>
#include <vector>

#include "action_status.h"
#include "build_types.h"
#include "download_types.h"
#include "glean_options.h"

namespace daw::glean {
	struct glean_file_item;

	class dependency {
		build_types_t m_build_type;
		std::string m_name;
		std::string m_uri;
		std::unique_ptr<glean_file_item> m_file_dep{};

	public:
		dependency( std::string const &name, build_types_t const &build_type,
		            std::string const &uri );

		dependency( std::string const &name, build_types_t const &build_type,
		            std::string const &uri, glean_file_item const &file_dep );

		dependency( dependency const &other );
		dependency &operator=( dependency const &other );
		~dependency( );
		dependency( dependency && ) noexcept = default;
		dependency &operator=( dependency &&other ) noexcept = default;

		std::string const &name( ) const noexcept;
		std::string const &uri( ) const noexcept;
		action_status build( ::daw::glean::build_types bt ) const;
		fs::path glean_file( ) const;
		action_status install( ::daw::glean::build_types bt ) const;
		std::vector<std::string> dep_names( ) const;
		size_t dep_count( ) const noexcept;
		int compare( dependency const &rhs ) const noexcept;
		glean_file_item const &file_dep( ) const noexcept;
		bool has_file_dep( ) const noexcept;
	};

	bool operator==( dependency const &lhs, dependency const &rhs ) noexcept;
	bool operator!=( dependency const &lhs, dependency const &rhs ) noexcept;
	bool operator<( dependency const &lhs, dependency const &rhs ) noexcept;
} // namespace daw::glean

namespace std {
	template<>
	struct hash<daw::glean::dependency> {
		inline size_t operator( )( ::daw::glean::dependency const &value ) const
		  noexcept {
			return std::hash<std::string>{}( value.name( ) );
		}
	};
} // namespace std

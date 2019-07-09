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

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "action_status.h"
#include "build_types.h"
#include "download_types.h"
#include "glean_options.h"

namespace daw::glean {

	class dependency {
		build_types_t m_build_type;
		download_types_t m_download_type;
		std::string m_name;
		std::string m_uri;

	public:
		inline dependency( std::string name, build_types_t build_type,
		                   download_types_t download_type, std::string uri )
		  : m_build_type( std::move( build_type ) )
		  , m_download_type( std::move( download_type ) )
		  , m_name( std::move( name ) )
		  , m_uri( std::move( uri ) ) {}

		inline std::string const &name( ) const noexcept {
			return m_name;
		}

		inline std::string const &uri( ) const noexcept {
			return m_uri;
		}

		inline action_status build( daw::build_types bt ) const {
			return m_build_type.build( bt );
		}

		inline fs::path glean_file( ) const {
			return {};
		}

		inline action_status install( daw::build_types bt ) const {
			return m_build_type.install( bt );
		}

		inline action_status download( ) const {
			return m_download_type.download( );
		}

		inline std::vector<std::string> dep_names( ) const {
			return {};
		}

		inline size_t dep_count( ) const noexcept {
			return dep_names( ).size( );
		}

		inline int compare( dependency const &rhs ) const noexcept {
			return m_name.compare( rhs.m_name );
		}

		inline download_types_t download_type( ) const noexcept {
			return m_download_type;
		}
	};

	inline bool operator==( dependency const &lhs,
	                        dependency const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	inline bool operator!=( dependency const &lhs,
	                        dependency const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	inline bool operator<( dependency const &lhs,
	                       dependency const &rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}
} // namespace daw::glean

namespace std {
	template<>
	struct hash<daw::glean::dependency> {
		inline size_t operator( )( daw::glean::dependency const &value ) const
		  noexcept {
			return std::hash<std::string>{}( value.name( ) );
		}
	};
} // namespace std

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

#include "utilities.h"

namespace daw::glean {
	enum class log_output_types { message, error };
	class logger {
		log_output_types m_out_type;

	public:
		constexpr logger( log_output_types out_type ) noexcept
		  : m_out_type( out_type ) {}

		logger const &operator( )( std::string const &message ) const;
		logger const &operator( )( std::wstring const &message ) const;
		logger const &operator( )( char c ) const;
		logger const &operator( )( wchar_t wc ) const;

		constexpr logger const &operator*( ) const {
			return *this;
		}

		template<typename T>
		logger const &operator=( T &&value ) const {
			return this->operator( )( std::forward<T>( value ) );
		}

		constexpr logger const &operator++( ) const {
			return *this;
		}

		constexpr logger const &operator++( int ) const {
			return *this;
		}
	};

	template<typename T>
	constexpr logger const &operator<<( logger const &l, T const &message ) {
		return l( message );
	}

	logger const &operator<<( logger const &l, fs::path const &path );
} // namespace daw::glean

inline constexpr daw::glean::logger log_message =
  daw::glean::logger( daw::glean::log_output_types::message );

inline constexpr daw::glean::logger log_error =
  daw::glean::logger( daw::glean::log_output_types::error );

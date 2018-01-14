// The MIT License (MIT)
//
// Copyright (c) 2016-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/utility/string_view.hpp>
#include <boost/filesystem/path.hpp>
#include <curl/curl.h>
#include <exception>
#include <mutex>

namespace daw {
	namespace glean {
		struct change_directory {
			boost::filesystem::path old_path;

			change_directory( boost::filesystem::path const & new_path );

			~change_directory( );

			change_directory( change_directory && ) = default;

			change_directory & operator=( change_directory && ) = default;

			change_directory( change_directory const & ) = delete;

			change_directory & operator=( change_directory const & ) = delete;
		};    // change_directory

		struct glean_exception: public std::runtime_error {
			glean_exception( boost::string_view msg );

			~glean_exception( );

			glean_exception( glean_exception const & ) = default;

			glean_exception( glean_exception && ) = default;

			glean_exception & operator=( glean_exception const & ) = default;

			glean_exception & operator=( glean_exception && ) = default;
		};    // glean_exception

		void verify_folder( boost::filesystem::path const & path );

		void verify_file( boost::filesystem::path const & f );

		class curl_t {
			static std::mutex m_init_lock;
			CURL * ptr;
		public:

			curl_t( ) noexcept;

			void close( ) noexcept;

			~curl_t( ) noexcept;

			operator CURL*() const noexcept;

			explicit operator bool( ) const noexcept;

			curl_t( curl_t const & ) = delete;
			curl_t & operator=( curl_t const & ) = delete;
			curl_t( curl_t && ) = default;
			curl_t & operator=( curl_t && ) = default;
		};	// curl_t

	}	// namespace glean
}    // namespace daw


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

#if __has_include( <filesystem> ) and __cpp_lib_filesystem
#define DAW_HAS_STDFILESYSTEM
#include <filesystem>
#else
#include <boost/filesystem/path.hpp>
#endif

#include <boost/filesystem.hpp>
#include <curl/curl.h>
#include <exception>
#include <mutex>
#include <sstream>

#include <daw/daw_string_view.h>

namespace daw::glean {
#ifdef DAW_HAS_STDFILESYSTEM
	namespace fs = std::filesystem;
#else
	namespace fs = boost::filesystem;
#endif

	namespace impl {
		std::mutex &get_curl_t_init_mutex( );
	} // namespace impl

	struct change_directory {
		fs::path old_path = fs::current_path( );

		inline change_directory( fs::path const &new_path ) {
			fs::current_path( new_path );
		}

		inline ~change_directory( ) noexcept {
			try {
				if( exists( old_path ) && is_directory( old_path ) ) {
					fs::current_path( old_path );
				}
			} catch( ... ) {}
		}

		change_directory( change_directory && ) noexcept = default;
		change_directory &operator=( change_directory && ) noexcept = default;

		change_directory( change_directory const & ) = delete;
		change_directory &operator=( change_directory const & ) = delete;
	}; // change_directory

	using glean_exception = std::runtime_error;

	inline void verify_folder( fs::path const &path ) {
		if( !exists( path ) ) {
			create_directories( path );
		}
		if( !exists( path ) || !is_directory( path ) ) {
			std::stringstream ss;
			ss << "Could not create folder (" << path << ") or is not a directory";
			throw glean_exception( ss.str( ) );
		}
	}

	inline void verify_file( fs::path const &f ) {
		if( exists( f ) && !is_regular_file( f ) ) {
			std::stringstream ss;
			ss << "File already exists but isn't a file (" << f << ")";
			throw glean_exception( ss.str( ) );
		}
	}

	class curl_t {
		CURL *ptr = nullptr;

	public:
		inline curl_t( ) noexcept {
			{
				std::lock_guard<std::mutex> lock( impl::get_curl_t_init_mutex( ) );
				curl_global_init( CURL_GLOBAL_DEFAULT );
			}
			ptr = curl_easy_init( );
		}
		inline ~curl_t( ) noexcept {
			close( );
		}

		curl_t( curl_t && ) noexcept = default;
		curl_t &operator=( curl_t && ) noexcept = default;
		curl_t( curl_t const & ) = delete;
		curl_t &operator=( curl_t const & ) = delete;

		inline void close( ) noexcept {
			if( auto tmp = std::exchange( ptr, nullptr ); tmp ) {
				try {
					curl_easy_cleanup( tmp );
					std::lock_guard<std::mutex> lock( impl::get_curl_t_init_mutex( ) );
					curl_global_cleanup( );
				} catch( ... ) {}
			}
		}

		inline operator CURL *( ) const noexcept {
			return ptr;
		}

		inline explicit operator bool( ) const noexcept {
			return ptr;
		}
	}; // curl_t

} // namespace daw::glean

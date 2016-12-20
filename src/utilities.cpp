// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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

#include <boost/utility/string_view.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <curl/curl.h>

#include "utilities.h"

namespace daw {
	namespace glean {
		change_directory::change_directory( boost::filesystem::path const & new_path ):
			old_path { boost::filesystem::current_path( ) } {

			boost::filesystem::current_path( new_path );
		}

		change_directory::~change_directory( ) {
			if( exists( old_path ) && is_directory( old_path ) ) {
				boost::filesystem::current_path( old_path );
			}
		}

		glean_exception::glean_exception( boost::string_view msg ):
			std::runtime_error { msg.data( ) } { }

		glean_exception::~glean_exception( ) { }

		void verify_folder( boost::filesystem::path const & path ) {
			if( !exists( path ) ) {
				create_directories( path );
			}
			if( !exists( path ) || !is_directory( path ) ) {
				std::stringstream ss;
				ss << "Could not create folder (" << path << ") or is not a directory";
				throw glean_exception( ss.str( ) );
			}
		}

		void verify_file( boost::filesystem::path const & f ) {
			if( exists( f ) && !is_regular_file( f ) ) {
				std::stringstream ss;
				ss << "File already exists but isn't a file (" << f << ")";
				throw glean_exception( ss.str( ) );

			}
		}

		curl_t::curl_t( ) noexcept:
			ptr { curl_easy_init( ) } { }

		constexpr curl_t::curl_t( CURL* p ) noexcept:
			ptr { p } { }

		void curl_t::close( ) noexcept {
			if( ptr ) {
				auto tmp = std::exchange( ptr, nullptr );
				curl_easy_cleanup( tmp );
			}
		}

		curl_t::~curl_t( ) noexcept {
			close( );
		}

		curl_t::operator void*() const noexcept {
			return ptr;
		}

		curl_t::operator bool( ) const noexcept {
			return nullptr != ptr;
		}
	}	// namespace glean
}    // namespace daw


// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <git2.h>
#include <iostream>
#include <string>

#include <daw/daw_utility.h>

#include "utilities.h"

namespace daw {
	class git_helper {
		git_repository *m_repos = nullptr;

	public:
		inline git_helper( ) noexcept {

			if( auto const features = git_libgit2_features( );
			    ( features & git_feature_t::GIT_FEATURE_HTTPS ) == 0 ) {

				std::cerr << "HTTPS support missing in libgit2\n";
			} else if( ( features & git_feature_t::GIT_FEATURE_SSH ) == 0 ) {
				std::clog << "SSH support missing in libgit2\n";
			}
			git_libgit2_init( );
		}

		inline void reset( ) noexcept {
			if( auto ptr = daw::exchange( m_repos, nullptr ); ptr ) {
				git_repository_free( ptr );
			}
		}

		git_helper( git_helper const & ) = delete;
		git_helper &operator=( git_helper const & ) = delete;

		inline git_helper( git_helper &&other ) noexcept
		  : m_repos( daw::exchange( m_repos, nullptr ) ) {}

		inline git_helper &operator=( git_helper &&rhs ) noexcept {
			if( this != &rhs ) {
				reset( );
				m_repos = daw::exchange( rhs.m_repos, nullptr );
			}
			return *this;
		}

		inline ~git_helper( ) noexcept {
			reset( );
			git_libgit2_shutdown( );
		}

		bool open_if_repos( glean::fs::path destination ) noexcept;
		int clone( std::string repos, glean::fs::path destination );
		int update( glean::fs::path destination );
		int checkout( glean::fs::path repos, std::string branch );
	};
} // namespace daw

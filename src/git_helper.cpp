// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <boost/filesystem.hpp>
#include <git2.h>
#include <git2/clone.h>
#include <iostream>
#include <string>

#include <daw/daw_string_fmt.h>

#include "git_helper.h"

namespace daw {
	git_helper::git_helper( )
	  : m_repos{nullptr} {

		git_libgit2_init( );
	}

	git_helper::git_helper( git_repository *repos )
	  : m_repos{repos} {

		git_libgit2_init( );
	}

	git_helper::~git_helper( ) noexcept {
		reset( );
	}

	void git_helper::reset( ) noexcept {
		auto tmp = std::exchange( m_repos, nullptr );
		if( tmp ) {
			git_repository_free( tmp );
		}
		git_libgit2_shutdown( );
	}

	namespace {
		int sideband_progress( const char *str, int len, void * ) {
			static daw::fmt_t const fmt1{"remote: {0}\n"};
			std::cout << fmt1( std::string{str, static_cast<size_t>( len )} );
			return EXIT_SUCCESS;
		}

		int fetch_progress( git_transfer_progress const *stats, void * ) {
			auto const fetch_percent = ( 100 * stats->received_objects ) / stats->total_objects;
			auto const index_percent = ( 100 * stats->indexed_objects ) / stats->total_objects;
			auto const kbytes = stats->received_bytes / 1024;
			static daw::fmt_t const f{"network {0} ({1} kb, {2}/{3}) / index {4} ({5}/{6})\n"};

			std::cout << f( fetch_percent, kbytes, stats->received_objects, stats->total_objects, index_percent,
			                stats->indexed_objects, stats->total_objects );
			return EXIT_SUCCESS;
		}

		void checkout_progress( const char *path, size_t cur, size_t tot, void * ) {
			static daw::fmt_t const f{"checkout: {0} - {1}\n"};
			std::cout << f( 100 * cur / tot, path );
		}

		int cred_acquire_cb( git_cred **cred, const char *url, const char *username_from_url, unsigned int allowed_types,
		                     void *payload ) {

			std::cerr << "Credentia support is not implemented" << std::endl;
			std::abort( );
		}
	} // namespace

	// TODO clone branch
	int git_helper::clone( std::string repos, boost::filesystem::path destination ) {
		static daw::fmt_t const msg{"No previous cache of {0}, cloning to {1}\n"};
		std::cout << msg( repos, destination.lexically_normal( ).string( ) );

		git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
		git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

		checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
		checkout_opts.progress_cb = &checkout_progress;
		checkout_opts.progress_payload = &repos;
		clone_opts.checkout_opts = checkout_opts;
		clone_opts.fetch_opts.callbacks.sideband_progress = &sideband_progress;
		clone_opts.fetch_opts.callbacks.transfer_progress = &fetch_progress;
		clone_opts.fetch_opts.callbacks.credentials = cred_acquire_cb;
		clone_opts.fetch_opts.callbacks.payload = &repos;

		int result = git_clone( &m_repos, repos.c_str( ), destination.lexically_normal( ).c_str( ), &clone_opts );
		if( result != 0 ) {
			git_error const * err = giterr_last( );
			std::cerr << "Error while cloning: " << err->message << '\n';
		}
		return result;
	}

	// TODO implement fetch/merge
	int git_helper::update( std::string repos, boost::filesystem::path destination ) {
		static daw::fmt_t const msg{"Existing cache found, updating {0}\n"};
		std::cout << msg( destination.lexically_normal( ).string( ) );

		if( exists( destination ) ) {
			remove_all( destination );
		}
		return clone( std::move( repos ), std::move( destination ) );
	}

} // namespace daw

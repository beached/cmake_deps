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

#include <git2.h>
#include <git2/clone.h>
#include <git2/cred_helpers.h>
#include <iostream>
#include <string>

#include <daw/daw_string_fmt.h>

#include "git_helper.h"
#include "utilities.h"

namespace daw {
	namespace {
		extern "C" int sideband_progress( const char *str, int len,
		                                  void * ) noexcept {
			static auto const f = daw::fmt_t( "remote: {0}\n" );
			std::cout << f( std::string( str, static_cast<size_t>( len ) ) );
			return EXIT_SUCCESS;
		}

		extern "C" int fetch_progress( git_transfer_progress const *stats,
		                               void * ) noexcept {
			auto const fetch_percent =
			  ( 100 * stats->received_objects ) / stats->total_objects;
			auto const index_percent =
			  ( 100 * stats->indexed_objects ) / stats->total_objects;
			auto const kbytes = stats->received_bytes / 1024;
			static auto const f =
			  daw::fmt_t( "network {0} ({1} kb, {2}/{3}) / index {4} ({5}/{6})\n" );

			std::cout << f( fetch_percent, kbytes, stats->received_objects,
			                stats->total_objects, index_percent,
			                stats->indexed_objects, stats->total_objects );
			return EXIT_SUCCESS;
		}

		extern "C" void checkout_progress( const char *path, size_t cur, size_t tot,
		                                   void * ) noexcept {
			static auto const f = daw::fmt_t( "checkout: {0} - {1}\n" );
			std::cout << "checkout: " << ( 100 * cur / tot) << " = " << path << '\n';
		}

		extern "C" int cred_acquire_cb( git_cred **cred, const char * /*url*/,
		                                const char *username_from_url,
		                                unsigned int allowed_types,
		                                void *payload ) {

			auto *userpass = reinterpret_cast<git_cred_userpass_payload *>( payload );
			if( !userpass or !userpass->password ) {
				return -1;
			}

			auto *effective_username = [&]( ) -> char const * {
				if( userpass->username ) {
					return userpass->username;
				} else {
					return username_from_url;
				}
				return nullptr;
			}( );

			if( !effective_username ) {
				return -1;
			}

			if( GIT_CREDTYPE_USERNAME & allowed_types ) {
				return git_cred_username_new( cred, effective_username );
			}

			if( ( GIT_CREDTYPE_USERPASS_PLAINTEXT & allowed_types ) == 0 or
			    git_cred_userpass_plaintext_new( cred, effective_username,
			                                     userpass->password ) < 0 ) {
				return -1;
			}

			return 0;
		}
	} // namespace

	// TODO clone branch
	int git_helper::clone( std::string repos, glean::fs::path destination ) {
		static auto const f =
		  daw::fmt_t( "No previous cache of '{0}', cloning to '{1}'\n" );

		std::cout << f( repos, destination.string( ) );
		if( !is_empty( destination ) ) {
			static auto const f2 = daw::fmt_t( "'{0}' is not empty, clearing\n" );
			std::cout << f2( destination.string( ) );
			remove_all( destination );
			create_directories( destination );
		}

		git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
		git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

		checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
		checkout_opts.progress_cb = &checkout_progress;
		checkout_opts.progress_payload = &repos;
		clone_opts.checkout_opts = checkout_opts;
		clone_opts.fetch_opts.callbacks.sideband_progress = &sideband_progress;
		clone_opts.fetch_opts.callbacks.transfer_progress = &fetch_progress;
		clone_opts.fetch_opts.callbacks.credentials = &cred_acquire_cb;
		clone_opts.fetch_opts.callbacks.payload = &repos;

		int result =
		  git_clone( &m_repos, repos.c_str( ), destination.c_str( ), &clone_opts );

		if( result != 0 ) {
			git_error const *err = giterr_last( );
			std::cerr << "Error while cloning: " << err->message << '\n';
		}
		return result;
	}

	// TODO implement fetch/merge
	int git_helper::update( std::string repos, glean::fs::path destination ) {
		static daw::fmt_t const msg( "Existing cache found, updating {0}\n" );
		std::cout << msg( destination.lexically_normal( ).string( ) );

		if( exists( destination ) ) {
			remove_all( destination );
		}
		return clone( std::move( repos ), std::move( destination ) );
	}

	int git_helper::checkout( glean::fs::path repos, std::string branch ) {
		if( !exists( repos ) ) {
			std::cerr << "Could not find valid repos at '" << repos << "'\n";
			exit( EXIT_FAILURE );
		}
		// TODO
		return 0;
	}
} // namespace daw

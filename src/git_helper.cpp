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

#include <libssh2.h>

#include <git2.h>

#include <git2/cred_helpers.h>
#include <iostream>
#include <string>

#include <daw/daw_string_fmt.h>

#include "git_helper.h"
#include "utilities.h"

namespace daw {
	namespace {
		int sideband_progress( const char *str, int len, void * ) noexcept {
			static auto const f = daw::fmt_t( "remote: {0}\n" );
			if( str and len > 0 ) {
				std::cout << f( std::string( str, static_cast<size_t>( len ) ) );
			}
			return EXIT_SUCCESS;
		}

		int fetch_progress( git_transfer_progress const *stats, void * ) noexcept {
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

		void checkout_progress( const char *path, size_t cur, size_t tot,
		                        void * ) noexcept {
			static auto const f = daw::fmt_t( "checkout: {0} - {1}\n" );
			std::cout << "checkout: " << ( 100 * cur / tot ) << " = "
			          << ( path ? path : "" ) << '\n';
		}

		int cred_acquire_cb( git_cred **cred, const char * /*url*/,
		                     const char *user_from_url, unsigned int allowed_types,
		                     void *payload ) {

			auto *userpass = reinterpret_cast<git_cred_userpass_payload *>( payload );
			if( !userpass or !userpass->password ) {
				return -1;
			}

			auto *effective_username = [&]( ) -> char const * {
				if( userpass->username ) {
					return userpass->username;
				} else if( user_from_url ) {
					return user_from_url;
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

		inline git_clone_options setup_clone_options( ) {
			git_clone_options clone_opts{};
			git_clone_init_options( &clone_opts, GIT_CLONE_OPTIONS_VERSION );

			git_checkout_init_options( &clone_opts.checkout_opts,
			                           GIT_CHECKOUT_OPTIONS_VERSION );

			clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
			clone_opts.checkout_opts.progress_cb = &checkout_progress;

			clone_opts.fetch_opts.callbacks.sideband_progress = &sideband_progress;
			clone_opts.fetch_opts.callbacks.transfer_progress = &fetch_progress;
			clone_opts.fetch_opts.callbacks.credentials = &cred_acquire_cb;
			return clone_opts;
		}
	} // namespace

	// TODO clone branch
	int git_helper::clone( std::string repos, glean::fs::path destination ) {
		if( exists( destination ) or is_directory( destination ) ) {
			static auto const f = daw::fmt_t(
			  "Previous cache of '{0}' found, deleting and cloning to '{1}'\n" );
			std::cout << f( repos, destination.string( ) );
			remove_all( destination );
			create_directory( destination );
		} else {
			static auto const f =
			  daw::fmt_t( "No previous cache of '{0}', cloning to '{1}'\n" );
			std::cout << f( repos, destination.string( ) );
		}
		if( !is_empty( destination ) ) {
			static auto const f2 = daw::fmt_t( "'{0}' is not empty, clearing\n" );
			std::cout << f2( destination.string( ) );
			remove_all( destination );
			create_directories( destination );
		}

		auto clone_opts = setup_clone_options( );
		clone_opts.checkout_opts.progress_payload = &repos;
		clone_opts.fetch_opts.callbacks.payload = &repos;

		int result =
		  git_clone( &m_repos, repos.c_str( ), destination.c_str( ), &clone_opts );

		if( result != 0 ) {
			git_error const *err = giterr_last( );
			std::cerr << "Error while cloning: " << ( err ? err->message : "" )
			          << '\n';
		}
		return result;
	}

	namespace {
		int fetch_update_cb( char const *refname, git_oid const *a,
		                     git_oid const *b, void * /*data*/ ) noexcept {

			auto b_str = std::array<char, GIT_OID_HEXSZ + 1>{};

			git_oid_fmt( b_str.data( ), b );
			if( git_oid_iszero( a ) ) {
				static auto const fmt = fmt_t( "[new]\t{0} {1}\n" );
				std::cout << fmt( b_str.data( ), refname );
			} else {
				std::array<char, GIT_OID_HEXSZ + 1> a_str{};
				git_oid_fmt( a_str.data( ), a );
				static auto const fmt = fmt_t( "[updated]\t{0} {1} {2}\n" );
				std::cout << fmt( a_str.data( ), b_str.data( ), refname );
			}
			return 0;
		}

		int fetch_transfer_progress_cb( git_transfer_progress const *stats,
		                                void * /*payload*/ ) noexcept {

			if( stats->received_objects == stats->total_objects ) {
				static auto const fmt = fmt_t( "Resolving deltas {0}/{1}\r" );
				std::cout << fmt( stats->indexed_deltas, stats->total_deltas );
			} else if( stats->total_objects > 0 ) {
				static auto const fmt =
				  fmt_t( "Received {0}/{1} objects ({2}) in {3} bytes\r" );
				std::cout << fmt( stats->received_objects, stats->total_objects,
				                  stats->indexed_objects, stats->received_bytes );
			}
			std::cout << '\n';
			return 0;
		}
		int fetchhead_ref_cb( const char * /*name*/, const char * /*url*/,
		                      const git_oid * /*oid*/, unsigned int is_merge,
		                      void * /*payload*/ ) {

			if( is_merge ) {
				// strcpy_s( branchToMerge, 100, name );
				// memcpy( &branchOidToMerge, oid, sizeof( git_oid ) );
			}
			return 0;
		}
	} // namespace

	// TODO implement fetch/merge
	int git_helper::update( glean::fs::path destination ) {
		int result = -1;
		if( !m_repos ) {
			result = git_repository_open( &m_repos, destination.c_str( ) );
			if( result != 0 ) {
				git_error const *err = giterr_last( );
				std::cerr << "Error while opening: '" << destination
				          << "': " << err->message << '\n';
				return result;
			}
		}

		git_reference *current_branch = nullptr;
		result = git_repository_head( &current_branch, m_repos );
		if( result != 0 ) {
			auto const *err = giterr_last( );
			std::cerr << "Error while opening repository head: '" << destination
			          << "': " << err->message << '\n';
			return result;
		}
		git_reference *upstream = nullptr;
		result = git_branch_upstream( &upstream, current_branch );
		if( result != 0 ) {
			auto const *err = giterr_last( );
			std::cerr << "Error while opening upstream branch: '" << destination
			          << "': " << err->message << '\n';
			return result;
		}
		auto remote_name = git_buf{nullptr, 0, 0};
		result = git_branch_remote_name( &remote_name, m_repos,
		                                 git_reference_name( upstream ) );
		if( result != 0 ) {
			auto const *err = giterr_last( );
			std::cerr << "Error while opening branch name: '" << destination
			          << "': " << err->message << '\n';
			return result;
		}
		git_remote *remote = nullptr;
		if( result = git_remote_lookup( &remote, m_repos, remote_name.ptr );
		    result != 0 ) {
			git_error const *err = giterr_last( );
			std::cerr << "Error while updating: '" << destination
			          << "': " << err->message << '\n';
			return result;
		}
		std::cout << "remote name: " << remote_name.ptr << '\n';
		git_buf_free( &remote_name );

		static daw::fmt_t const msg( "Existing cache found, updating {0}\n" );
		auto remote_url = daw::string_view( git_remote_url( remote ) ).to_string( );
		std::cout << msg( remote_url );

		auto fetch_opts = git_fetch_options{};
		git_fetch_init_options( &fetch_opts, GIT_FETCH_OPTIONS_VERSION );
		git_remote_init_callbacks( &fetch_opts.callbacks,
		                           GIT_REMOTE_CALLBACKS_VERSION );
		//		fetch_opts.callbacks.sideband_progress = &sideband_progress;
		fetch_opts.callbacks.transfer_progress = &fetch_transfer_progress_cb;
		//		fetch_opts.callbacks.update_tips = &fetch_update_cb;
		fetch_opts.callbacks.credentials = &cred_acquire_cb;
		fetch_opts.callbacks.payload = &remote_url;
		// fetch_opts.prune = git_fetch_prune_t::GIT_FETCH_PRUNE;
		// fetch_opts.download_tags =
		//  git_remote_autotag_option_t::GIT_REMOTE_DOWNLOAD_TAGS_ALL;

		result = git_remote_fetch( remote, nullptr, &fetch_opts, nullptr );
		if( result != 0 ) {
			git_error const *err = giterr_last( );
			std::cerr << "Error while fetching: '" << destination
			          << "': " << ( err ? err->message : "" ) << '\n';
			return result;
		}
		/*
		    git_repository_fetchhead_foreach( m_repos, fetchhead_ref_cb, nullptr
		   ); auto stats = git_remote_stats( remote ); if( stats->local_objects >
		   0 ) { auto const fmt = fmt_t( "Received {0}/{1} objects in {2} bytes
		   (used {3} local objects)\n"
		   ); std::cout << fmt( stats->indexed_objects, stats->total_objects,
		                        stats->received_bytes, stats->local_objects );
		    } else {
		      auto const fmt = fmt_t( "\rReceived {0}/{1} objects in {2} bytes\n"
		   ); std::cout << fmt( stats->indexed_objects, stats->total_objects,
		                        stats->received_bytes );
		    }
		    // Merge
		    std::cout << '\n';
		    */
		return result;
	}

	int git_helper::checkout( glean::fs::path repos, std::string branch ) {
		if( !exists( repos ) ) {
			std::cerr << "Could not find valid repos at '" << repos << "'\n";
			exit( EXIT_FAILURE );
		}
		// TODO
		return 0;
	}

	bool git_helper::open_if_repos( glean::fs::path destination ) noexcept {
		return git_repository_open_ext( &m_repos, destination.c_str( ),
		                                GIT_REPOSITORY_OPEN_NO_SEARCH,
		                                nullptr ) == 0;
	}
} // namespace daw

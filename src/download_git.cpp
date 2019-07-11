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

#include <string>
#include <utility>

#include "daw/glean/action_status.h"
#include "daw/glean/download_git.h"
#include "daw/glean/git_helper.h"
#include "daw/glean/logging.h"
#include "daw/glean/proc.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	namespace {
		bool is_git_repos( fs::path repos ) {
			return is_directory( repos / ".git" );
		}

		action_status git_repos_checkout( fs::path repos,
		                                  std::string const &version ) {
			auto const chdir = change_directory( repos );
			auto result = git_runner( git_action_reset( ), repos, log_message );
			if( result == action_status::success ) {
				if( version.empty( ) ) {
					result = git_runner( git_action_version{"master"}, repos,
					                     log_message );
				} else {
					result =
					  git_runner( git_action_version{version}, repos, log_message );
				}
			}
			return result;
		}

		action_status git_repos_update( fs::path repos ) {
			auto const chdir = change_directory( repos );
			// Clean out any changes
			auto result = git_runner( git_action_reset( ), repos, log_message );
			if( result == action_status::success ) {
				result = git_runner( git_action_pull{}, repos, log_message );
			}
			return result;
		}

		action_status git_repos_clone( std::string const &remote_repos,
		                               fs::path repos ) {

			auto git_action = git_action_clone( );
			git_action.remote_uri = remote_repos;

			return git_runner( git_action, repos, log_message );
		}

	} // namespace

	action_status download_git::download( ) const {
		action_status result = action_status::failure;
		if( is_git_repos( m_local ) ) {
			result = git_repos_update( m_local );
		} else {
			result = git_repos_clone( m_remote, m_local );
		}
		if( result == action_status::success ) {
			return git_repos_checkout( m_local, m_version );
		}
		return result;
	}
} // namespace daw::glean

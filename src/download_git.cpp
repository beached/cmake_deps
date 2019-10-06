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
		[[nodiscard]] bool is_git_repos( fs::path const &repos ) {
			return is_directory( repos / ".git" );
		}

		[[nodiscard]] action_status
		git_repos_checkout( fs::path const &repos, std::string const &version ) {
			auto const chdir = change_directory( repos );
			auto result = git_runner( git_action_reset( ), repos, log_message );
			if( result == action_status::success ) {
				if( version.empty( ) ) {
					result =
					  git_runner( git_action_version{"master"}, repos, log_message );
				} else {
					result =
					  git_runner( git_action_version{version}, repos, log_message );
				}
			}
			return result;
		}

		[[nodiscard]] action_status git_repos_update( fs::path const &repos ) {
			auto const chdir = change_directory( repos );
			// Clean out any changes
			auto result = git_runner( git_action_reset( ), repos, log_message );
			if( result == action_status::success ) {
				result = git_runner( git_action_pull{}, repos, log_message );
			}
			return result;
		}

		[[nodiscard]] action_status
		git_repos_clone( std::string const &remote_repos, fs::path repos ) {

			auto git_action = git_action_clone( );
			git_action.remote_uri = remote_repos;

			return git_runner( git_action, std::move( repos ), log_message );
		}

	} // namespace

	action_status download_git::download( glean_file_item const &dep,
	                                      fs::path const &cache_folder ) const {
		action_status result = action_status::failure;
		auto repos = cache_folder / "source";
		if( is_git_repos( repos ) ) {
			log_message << "git update of '" << repos << "'\n";
			result = git_repos_update( repos );
		} else {
			log_message << "git clone of '" << dep.uri << "' into '" << repos
			            << "'\n";
			result = git_repos_clone( dep.uri, repos );
		}
		if( to_bool( result ) ) {
			log_message << "git checkout with '" << repos << "' to " << dep.version
			            << '\n';
			return git_repos_checkout( repos, dep.version );
		}
		return result;
	}
} // namespace daw::glean

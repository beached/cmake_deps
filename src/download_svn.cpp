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
#include "daw/glean/download_svn.h"
#include "daw/glean/logging.h"
#include "daw/glean/proc.h"
#include "daw/glean/svn_helper.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	namespace {
		[[nodiscard]] bool is_svn_repos( fs::path const &repos ) {
			return is_directory( repos / ".svn" );
		}

		[[nodiscard]] action_status svn_repos_update( fs::path repos ) {
			return svn_runner( svn_action_update{}, std::move( repos ), log_message );
		}

		[[nodiscard]] action_status
		svn_repos_checkout( std::string const &remote_repos, fs::path repos ) {

			auto svn_action = svn_action_checkout( );
			svn_action.remote_uri = remote_repos;

			return svn_runner( svn_action, std::move( repos ), log_message );
		}

	} // namespace

	action_status download_svn::download( glean_file_item const &dep,
	                                      fs::path const &cache_folder ) const {
		auto result = action_status::failure;
		auto repos = cache_folder / "source";
		if( is_svn_repos( repos ) ) {
			log_message << "svn update of '" << repos << "'\n";
			result = svn_repos_update( repos );
		} else {
			log_message << "svn clone of '" << dep.uri << "' into '" << repos
			            << "'\n";
			result = svn_repos_checkout( dep.uri, repos );
		}
		if( to_bool( result ) and not dep.version.empty( ) ) {
			// Set repos to version
			log_error << "version selection in svn not yet supported\n";
		}
		return result;
	}
} // namespace daw::glean

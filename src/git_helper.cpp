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

#include <string>
#include <vector>

#include "daw/glean/git_helper.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	std::vector<std::string>
	git_action_pull::build_args( fs::path work_tree ) const {
		std::string work_tree_str = "--work-tree=";
		work_tree_str += work_tree.string( );
		return {work_tree_str, "pull"};
	}

	std::vector<std::string>
	git_action_clone::build_args( fs::path work_tree ) const {
		std::vector<std::string> result = {"clone"};
		if( recurse_submodules ) {
			result.push_back( "--recurse-submodules" );
		}
		result.push_back( remote_uri );
		result.push_back( work_tree.string( ) );
		return result;
	}
} // namespace daw::glean

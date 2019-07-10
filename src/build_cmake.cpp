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

#include <utility>

#include "daw/glean/build_cmake.h"
#include "daw/glean/cmake_helper.h"
#include "daw/glean/glean_options.h"
#include "daw/glean/logging.h"
#include "daw/glean/proc.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	template<size_t pos = 0, typename Runner, typename Action,
	         typename... Actions>
	constexpr std::pair<size_t, action_status>
	run_many( Runner r, Action &&action, Actions &&... actions ) {
		if( r( std::forward<Action>( action ) ) == action_status::success ) {
			if constexpr( sizeof...( Actions ) > 0 ) {
				return run_many<pos + 1>( std::move( r ),
				                          std::forward<Actions>( actions )... );
			} else {
				return {pos, action_status::success};
			}
		}
		return {pos, action_status::failure};
	}

	build_cmake::build_cmake( fs::path source_path, fs::path build_path,
	                          fs::path install_prefix,
	                          glean_options const &opts ) noexcept
	  : m_source_path( std::move( source_path ) )
	  , m_build_path( std::move( build_path ) )
	  , m_install_prefix( std::move( install_prefix ) )
	  , m_opt( &opts ) {}

	action_status build_cmake::build( daw::glean::build_types bt ) const {
		auto const chdir = change_directory( m_build_path );
		if( cmake_runner( cmake_action_configure( m_source_path, m_install_prefix, m_opt->cmake_args() ),
		                  m_build_path, bt,
		                  log_message ) == action_status::failure ) {

			return action_status::failure;
		}
		return cmake_runner( cmake_action_build( ), m_build_path, bt, log_message );
	}

	action_status build_cmake::install( daw::glean::build_types bt ) const {
		auto const chdir = change_directory( m_build_path );
		return cmake_runner( cmake_action_install( ), m_build_path, bt,
		                     log_message );
	}
} // namespace daw::glean

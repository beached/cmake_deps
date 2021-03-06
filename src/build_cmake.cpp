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
#include <vector>

#include "daw/glean/build_cmake.h"
#include "daw/glean/cmake_helper.h"
#include "daw/glean/glean_file.h"
#include "daw/glean/glean_file_item.h"
#include "daw/glean/glean_options.h"
#include "daw/glean/logging.h"
#include "daw/glean/proc.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	template<size_t pos = 0, typename Runner, typename Action,
	         typename... Actions>
	[[nodiscard]] constexpr std::pair<size_t, action_status>
	run_many( Runner r, Action &&action, Actions &&... actions ) {
		if( to_bool( r( std::forward<Action>( action ) ) ) ) {
			if constexpr( sizeof...( Actions ) > 0 ) {
				return run_many<pos + 1>( std::move( r ),
				                          std::forward<Actions>( actions )... );
			} else {
				return {pos, action_status::success};
			}
		}
		return {pos, action_status::failure};
	}

	build_cmake::build_cmake( fs::path const &cache_path,
	                          fs::path const &install_prefix,
	                          glean_options const &opts, bool has_glean ) noexcept
	  : m_cache_path( cache_path )
	  , m_install_prefix( install_prefix )
	  , m_opt( &opts )
	  , m_has_glean( has_glean ) {}

	action_status build_cmake::build( daw::glean::build_types bt,
	                                  glean_file_item const &m_dep_item ) const {
		assert( m_opt != nullptr );
		auto const chdir = change_directory( m_cache_path / "build" );
		auto args = std::vector<std::string>( );
		std::copy_if( m_opt->cmake_args.cbegin( ), m_opt->cmake_args.cend( ),
		              std::back_inserter( args ),
		              []( std::string const &s ) { return not s.empty( ); } );
		std::copy_if( m_dep_item.cmake_args.cbegin( ),
		              m_dep_item.cmake_args.cend( ), std::back_inserter( args ),
		              []( std::string const &s ) { return not s.empty( ); } );
		if( m_opt->build_type == daw::glean::build_types::debug ) {
			args.push_back( "-DCMAKE_BUILD_TYPE=Debug" );
		} else {
			args.push_back( "-DCMAKE_BUILD_TYPE=Release" );
		}

		if( not to_bool( cmake_runner(
		      cmake_action_configure( m_cache_path / "source", m_install_prefix,
		                              std::move( args ), m_has_glean ),
		      m_cache_path / "build", bt, log_message ) ) ) {

			return action_status::failure;
		}
		return cmake_runner( cmake_action_build( m_opt->jobs ),
		                     m_cache_path / "build", bt, log_message );
	}

	action_status build_cmake::install( daw::glean::build_types bt ) const {
		auto const chdir = change_directory( m_cache_path / "build" );
		return cmake_runner( cmake_action_install( ), m_cache_path / "build", bt,
		                     log_message );
	}
} // namespace daw::glean

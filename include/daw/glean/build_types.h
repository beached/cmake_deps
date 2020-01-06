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

#pragma once

#include <daw/daw_enable_if.h>
#include <daw/daw_traits.h>
#include <daw/daw_visit.h>
#include <daw/glean/impl/build_types_impl.h>

#include "action_status.h"
#include "build_cmake.h"
#include "build_none.h"
#include "glean_options.h"
#include "impl/build_types_impl.h"

namespace daw::glean {
	template<typename... BuildTypes>
	class basic_build_types {
		std::variant<BuildTypes...> m_value;

		template<typename T>
		static inline std::variant<BuildTypes...>
		construct_bt( daw::string_view type, fs::path const &cache_path,
		              fs::path const &install_prefix, glean_options const &opts,
		              bool has_glean ) {
			assert( T::type_id == type );
			return daw::construct_a<T>( cache_path, install_prefix, opts, has_glean );
		}

		template<typename T, typename Ts, typename... Ts2>
		static inline std::variant<BuildTypes...>
		construct_bt( daw::string_view type, fs::path const &cache_path,
		              fs::path const &install_prefix, glean_options const &opts,
		              bool has_glean ) {
			if( T::type_id == type ) {
				return daw::construct_a<T>( cache_path, install_prefix, opts,
				                            has_glean );
			} else {
				return construct_bt<Ts, Ts2...>( type, cache_path, install_prefix, opts,
				                                 has_glean );
			}
		}

	public:
		template<typename BuildType,
		         daw::enable_when_t<daw::traits::is_one_of_v<
		           daw::remove_cvref_t<BuildType>, BuildTypes...>> = nullptr>
		constexpr basic_build_types( BuildType &&dep )
		  : m_value( std::forward<BuildType>( dep ) ) {}

		static_assert( ( daw::glean::impl::can_construct_build_type_v<
		                   BuildTypes, fs::path const &, fs::path const &,
		                   glean_options const &, bool> and
		                 ... ),
		               "All build types must support construction" );
		inline basic_build_types( daw::string_view type, fs::path const &cache_path,
		                          fs::path const &install_prefix,
		                          glean_options const &opts, bool has_glean )
		  : m_value( construct_bt<BuildTypes...>( type, cache_path, install_prefix,
		                                          opts, has_glean ) ) {}

		static_assert(
		  ( daw::glean::impl::has_build_method_v<
		      BuildTypes, daw::glean::build_types, glean_file_item const &> and
		    ... ),
		  "All build types must support build method" );
		[[nodiscard]] constexpr action_status
		build( daw::glean::build_types bt, glean_file_item const &file_dep ) const {

			return daw::visit_nt(
			  m_value, [&]( auto const &v ) { return v.build( bt, file_dep ); } );
		}

		static_assert( ( daw::glean::impl::has_install_method_v<
		                   BuildTypes, daw::glean::build_types> and
		                 ... ),
		               "All install types must support install method" );
		[[nodiscard]] constexpr action_status
		install( daw::glean::build_types bt ) const {

			return daw::visit_nt( m_value,
			                      [bt]( auto const &v ) { return v.install( bt ); } );
		}
	};

	using build_types_t = basic_build_types<build_none, build_cmake>;
} // namespace daw::glean

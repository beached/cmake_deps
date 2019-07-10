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

#include "action_status.h"
#include "build_cmake.h"
#include "build_none.h"
#include "glean_options.h"

namespace daw::glean {
	template<typename... BuildTypes>
	class basic_build_types {
		std::variant<BuildTypes...> m_value;

		template<typename T, typename... Ts, typename... Args,
		         daw::enable_if_t<( sizeof...( Ts ) < 1 )> = nullptr>
		static constexpr std::variant<BuildTypes...>
		construct_bt( daw::string_view type, Args &&... args ) {
			if( T::type_id == type ) {
				if constexpr( std::is_constructible_v<T, Args...> ) {
					return {T( std::forward<Args>( args )... )};
				} else {
					std::abort( );
				}
			}
			std::abort( );
		}

		template<typename T, typename... Ts, typename... Args,
		         daw::enable_if_t<( sizeof...( Ts ) > 0 )> = nullptr>
		static constexpr std::variant<BuildTypes...>
		construct_bt( daw::string_view type, Args &&... args ) {
			if( T::type_id == type ) {
				if constexpr( std::is_constructible_v<T, Args...> ) {
					return {T( std::forward<Args>( args )... )};
				} else {
					std::abort( );
				}
			}
			return construct_bt<Ts...>( type, std::forward<Args>( args )... );
		}

	public:
		template<typename T, daw::enable_if_t<daw::traits::is_one_of_v<
		                       daw::remove_cvref_t<T>, BuildTypes...>> = nullptr>
		constexpr basic_build_types( T &&dep )
		  : m_value( std::forward<T>( dep ) ) {}

		template<typename... Args>
		constexpr basic_build_types( daw::string_view type, Args &&... args )
		  : m_value(
		      construct_bt<BuildTypes...>( type, std::forward<Args>( args )... ) ) {
		}

		constexpr action_status build( daw::build_types bt ) const {
			return daw::visit_nt( m_value,
			                      [bt]( auto const &v ) { return v.build( bt ); } );
		}

		constexpr action_status install( daw::build_types bt ) const {
			return daw::visit_nt( m_value,
			                      [bt]( auto const &v ) { return v.install( bt ); } );
		}
	};

	using build_types_t = basic_build_types<build_none, build_cmake>;
} // namespace daw::glean

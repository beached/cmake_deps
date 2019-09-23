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

#include <utility>
#include <variant>

#include <daw/daw_enable_if.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_visit.h>

#include "action_status.h"
#include "download_git.h"
#include "download_none.h"
#include "download_svn.h"

namespace daw::glean {
	template<typename... DownloadTypes>
	class basic_download_types {
		static_assert( sizeof...( DownloadTypes ) > 0 );

		using variant_t = std::variant<DownloadTypes...>;
		variant_t m_value;

		template<typename T>
		static constexpr variant_t construct_dt( daw::string_view type ) {
			if( T::type_id == type ) {
				return daw::construct_a<T>( );
			}
			std::abort( );
		}

		template<typename T, typename... Ts,
		         daw::enable_when_t<( sizeof...( Ts ) > 0 )> = nullptr>
		static constexpr variant_t construct_dt( daw::string_view type ) {
			if( T::type_id == type ) {
				return daw::construct_a<T>( );
			}
			return construct_dt<Ts...>( type );
		}

	public:
		template<typename T, daw::enable_when_t<daw::traits::is_one_of_v<
		                       daw::remove_cvref_t<T>, DownloadTypes...>> = nullptr>
		constexpr basic_download_types( T &&dep )
		  : m_value( std::forward<T>( dep ) ) {}

		constexpr basic_download_types( daw::string_view type )
		  : m_value( construct_dt<DownloadTypes...>( type ) ) {}

		inline action_status download( glean_file_item const &dep,
		                               fs::path const &cache_folder ) const {
			return daw::visit_nt( m_value, [&]( auto const &v ) {
				return v.download( dep, cache_folder );
			} );
		}

		constexpr daw::string_view type_id( ) const noexcept {
			return daw::visit_nt( m_value,
			                      []( auto const &v ) { return v.type_id; } );
		}
	};

	using download_types_t =
	  basic_download_types<download_none, download_git, download_svn>;
} // namespace daw::glean

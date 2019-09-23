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

#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

namespace daw::glean::impl {
	template<typename T, typename... Args>
	using has_build_method_detect =
	  decltype( std::declval<T>( ).build( std::declval<Args>( )... ) );

	template<typename T, typename... Args>
	inline constexpr bool has_build_method_v =
	  daw::is_detected_v<has_build_method_detect, T, Args...>;

	template<typename T, typename... Args>
	using has_install_method_detect =
	  decltype( std::declval<T>( ).install( std::declval<Args>( )... ) );

	template<typename T, typename... Args>
	inline constexpr bool has_install_method_v =
	  daw::is_detected_v<has_install_method_detect, T, Args...>;

	template<typename T, typename... Args>
	using can_construct_build_type_detect =
	  decltype( daw::construct_a<T>( std::declval<Args>( )... ) );

	template<typename T, typename... Args>
	inline constexpr bool can_construct_build_type_v =
	  daw::is_detected_v<can_construct_build_type_detect, T, Args...>;
} // namespace daw::glean::impl

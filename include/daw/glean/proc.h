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

#include <algorithm>
#include <boost/process.hpp>
#include <string>

#include "utilities.h"

namespace daw::glean {
	template<typename OutputIterator>
	class Process {
		OutputIterator m_out;

	public:
		constexpr Process( OutputIterator out ) noexcept
		  : m_out( out ) {}

		template<typename Cmd, typename... Args>
		int operator( )( Cmd &&cmd, Args &&... args ) {
			auto out = boost::process::ipstream( );
			auto err = boost::process::ipstream( );

			auto git_proc = boost::process::child(
			  boost::process::search_path( cmd ), std::forward<Args>( args )...,
			  boost::process::std_out > out, boost::process::std_err > err );

			auto const process_pipe = [&]( auto &&p ) -> bool {
				typename glean::fs::path::string_type line{};
				if( !p or !std::getline( p, line ) or line.empty( ) ) {
					return false;
				}
				line += static_cast<typename glean::fs::path::value_type>( '\n' );
				m_out = std::copy( line.begin( ), line.end( ), m_out );
				return true;
			};
			while( git_proc and ( process_pipe( err ) or process_pipe( out ) ) ) {}

			git_proc.wait( );
			return git_proc.exit_code( );
		}
	};
} // namespace daw::glean

// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/filesystem/path.hpp>
#include <string>

namespace daw {
	struct git_helper {
		git_repository * m_repos;

		git_helper( );
		git_helper( git_repository * repos );

		git_helper( git_helper const & ) = delete;
		git_helper &operator=( git_helper const & ) = delete;
		git_helper( git_helper && ) = default;
		git_helper &operator=( git_helper && ) = default;

		~git_helper( ) noexcept;
		void reset( ) noexcept;

		int clone( std::string repos, boost::filesystem::path destination );
		int update( std::string repos, boost::filesystem::path destination );
		int checkout( boost::filesystem::path repos, std::string branch );
	};


}    // namespace daw


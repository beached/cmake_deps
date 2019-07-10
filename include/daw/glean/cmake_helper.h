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

#include <iostream>
#include <string>
#include <vector>

#include <daw/daw_utility.h>

#include "action_status.h"
#include "glean_options.h"
#include "proc.h"
#include "utilities.h"

namespace daw::glean {
	template<typename CmakeAction, typename OutputIterator>
	action_status cmake_runner( CmakeAction &&cmake_action, fs::path work_tree,
	                            daw::glean::build_types bt,
	                            OutputIterator &&out_it ) {
		auto args = cmake_action.build_args( std::move( work_tree ), bt );
		std::cout << "Running cmake";
		for( auto arg : args ) {
			std::cout << ' ' << arg;
		}
		std::cout << "\n\n";

		auto run_process = Process( std::forward<OutputIterator>( out_it ) );
		if( run_process( "cmake", std::move( args ) ) == EXIT_SUCCESS ) {
			return action_status::success;
		}
		return action_status::failure;
	}

	struct cmake_action_configure {
		fs::path source_path;
		fs::path install_prefix;
		std::vector<std::string> custom_arguments{};

		inline cmake_action_configure( fs::path source, fs::path install ) noexcept
		  : source_path( source )
		  , install_prefix( install ) {}

		inline cmake_action_configure( fs::path source, fs::path install,
		                               std::vector<std::string> custom ) noexcept
		  : source_path( source )
		  , install_prefix( install )
		  , custom_arguments( std::move( custom ) ) {}

		std::vector<std::string> build_args( fs::path build_path,
		                                     daw::glean::build_types bt ) const;
	};

	struct cmake_action_build {
		std::vector<std::string> build_args( fs::path build_path,
		                                     daw::glean::build_types bt ) const;
	};

	struct cmake_action_install {
		std::vector<std::string> build_args( fs::path build_path,
		                                     daw::glean::build_types bt ) const;
	};
} // namespace daw::glean

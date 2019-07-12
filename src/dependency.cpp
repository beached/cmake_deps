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

#include <cstdint>
#include <memory>
#include <string>

#include "daw/glean/action_status.h"
#include "daw/glean/dependency.h"
#include "daw/glean/glean_file.h"
#include "daw/glean/utilities.h"

namespace daw::glean {
	dependency::dependency( std::string const &name,
	                        build_types_t const &build_type,
	                        std::string const &uri )
	  : m_build_type( build_type )
	  , m_name( name )
	  , m_uri( uri ) {}

	dependency::dependency( std::string const &name,
	                        build_types_t const &build_type,
	                        std::string const &uri,
	                        glean_file_item const &file_dep )
	  : m_build_type( build_type )
	  , m_name( name )
	  , m_uri( uri )
	  , m_file_dep( std::make_unique<glean_file_item>( file_dep ) ) {}

	dependency::dependency( const dependency &other )
	  : m_build_type( other.m_build_type )
	  , m_name( other.m_name )
	  , m_uri( other.m_uri )
	  , m_file_dep( other.m_file_dep
	                  ? std::make_unique<glean_file_item>( *other.m_file_dep )
	                  : std::unique_ptr<glean_file_item>( ) ) {}

	dependency &dependency::operator=( const dependency &other ) {
		m_build_type = other.m_build_type;
		m_name = other.m_name;
		m_uri = other.m_uri;
		m_file_dep = other.m_file_dep
		               ? std::make_unique<glean_file_item>( *other.m_file_dep )
		               : std::unique_ptr<glean_file_item>( );
		return *this;
	}

	dependency::~dependency( ) {}

	std::string const &dependency::name( ) const noexcept {
		return m_name;
	}

	std::string const &dependency::uri( ) const noexcept {
		return m_uri;
	}

	action_status dependency::build( ::daw::glean::build_types bt ) const {
		assert( m_file_dep != nullptr );
		return m_build_type.build( bt, *m_file_dep );
	}

	fs::path dependency::glean_file( ) const {
		std::abort( );
		//		return {};
	}

	action_status dependency::install( ::daw::glean::build_types bt ) const {
		return m_build_type.install( bt );
	}

	std::vector<std::string> dependency::dep_names( ) const {
		std::abort( );
		//		return {};
	}

	size_t dependency::dep_count( ) const noexcept {
		return dep_names( ).size( );
	}

	int dependency::compare( dependency const &rhs ) const noexcept {
		return m_name.compare( rhs.m_name );
	}

	bool dependency::has_file_dep( ) const noexcept {
		return static_cast<bool>( m_file_dep );
	}

	glean_file_item const &dependency::file_dep( ) const noexcept {
		assert( m_file_dep != nullptr );
		return *m_file_dep;
	}

	bool operator==( dependency const &lhs, dependency const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	bool operator!=( dependency const &lhs, dependency const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	bool operator<( dependency const &lhs, dependency const &rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}
} // namespace daw::glean

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
#include <optional>
#include <string>
#include <vector>

#include "daw/glean/action_status.h"
#include "daw/glean/dependency.h"
#include "daw/glean/glean_file_item.h"
#include "daw/glean/utilities.h"

namespace daw::glean {

	dependency::item_t const &dependency::alt( ) const {
		assert( m_alternatives.size( ) > m_index );
		return m_alternatives[m_index];
	}

	dependency::item_t &dependency::alt( ) {
		assert( m_alternatives.size( ) > m_index );
		return m_alternatives[m_index];
	}

	dependency::dependency( std::string const &name,
	                        build_types_t const &build_type )
	  : m_name( name )
	  , m_alternatives{{build_type}} {}

	dependency::dependency( std::string const &name,
	                        build_types_t const &build_type,
	                        glean_file_item const &file_dep )
	  : m_name( name )
	  , m_alternatives{{build_type, file_dep}} {}

	std::string const &dependency::name( ) const noexcept {
		return m_name;
	}

	action_status dependency::build( ::daw::glean::build_types bt ) const {
		assert( alt( ).file_dep );
		return alt( ).build_type.build( bt, *( alt( ).file_dep ) );
	}

	action_status dependency::install( ::daw::glean::build_types bt ) const {
		return alt( ).build_type.install( bt );
	}

	bool dependency::has_file_dep( ) const noexcept {
		return static_cast<bool>( alt( ).file_dep );
	}

	glean_file_item const &dependency::file_dep( ) const noexcept {
		assert( alt( ).file_dep );
		return *( alt( ).file_dep );
	}

	std::vector<dependency::item_t> &dependency::alternatives( ) noexcept {
		return m_alternatives;
	}

	std::vector<dependency::item_t> const &dependency::alternatives( ) const noexcept {
		return m_alternatives;
	}
} // namespace daw::glean

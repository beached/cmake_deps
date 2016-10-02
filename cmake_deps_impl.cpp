
// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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

#include <boost/utility/string_ref.hpp>
#include <exception>
#include <ostream>

#include "cmake_deps_impl.h"

namespace daw {
	namespace cmake_deps {
		namespace {
			struct cmake_deps_exception: public std::runtime_error {
				cmake_deps_exception( boost::string_ref msg ):
					std::runtime_error{ msg.data( ) } { }
			};	// cmake_deps_exception

			std::ostream & operator<<( std::ostream & os, cmake_deps_item const & item ) {
				return (os << item.to_string( ));
			}
			
			bool is_update_needed( cmake_deps_item const & item ) {
				return true;
			}

			void clone( cmake_deps_item const & item ) {

			}

			void build( cmake_deps_item const & item ) {

			}

			void install( cmake_deps_item const & item ) {

			}

			void process_item( cmake_deps_item const & item ) {
				if( is_update_needed( item ) ) {
					clone( item );
					build( item );
					install( item );
				}
			}
		}

		void process_file( cmake_deps_file const & depend_file ) {
			for( auto const & dependency : depend_file.dependencies ) {
				std::cout << "Processing: " << dependency << '\n';
				try {
					process_item( dependency );
				} catch( cmake_deps_exception const & ex ) {
					std::cerr << "Error processing: " << dependency << ":\n" << ex.what( ) << std::endl;
				}
			}
		}
	}	// namespace cmake_deps
}    // namespace daw


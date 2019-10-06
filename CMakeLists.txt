cmake_minimum_required( VERSION 3.0.2 )

set( CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")

project( glean_prj )

if( USE_STATIC_BOOST ) 
	set( Boost_USE_STATIC_LIBS ON )
endif( )

find_package( Boost 1.62.0 COMPONENTS program_options iostreams filesystem REQUIRED )
find_package( Threads REQUIRED )

enable_testing( )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

set( DEP_ROOT_DIR_BASE "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/debug" )
	set( GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/debug" )
	set( GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/release" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
	set( GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/release" )
	set( GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/debug" )
else( )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
	set( GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/release" )
	set( GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/debug" )
endif( )

message( "Building for: ${CMAKE_BUILD_TYPE}" )
message( "Checking for glean cache: '${GLEAN_CACHE}'" )
if( EXISTS "${GLEAN_CACHE}" )
	message( "using glean dependencies" )

	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${GLEAN_CACHE}/include" )
	link_directories( "${GLEAN_CACHE}/lib" )
elseif( EXISTS "${GLEAN_CACHE2}" )
	message( "looks like glean is in use but missing ${GLEAN_CACHE}" )
elseif( EXISTS "${CMAKE_SOURCE_DIR}/glean.cmake" )
	message( "glean.cmake detected" )
	include( "${CMAKE_SOURCE_DIR}/glean.cmake" )
	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )
	include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
	link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
elseif( EXISTS "${DEP_ROOT_DIR}" )
	message( "${DEP_ROOT_DIR}" )
	message( "using dependency folder" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${DEP_ROOT_DIR}/install/include" )
	link_directories( "${DEP_ROOT_DIR}/install/lib" )
else( )
	message( "downloading and building deps" )
	include( "${CMAKE_SOURCE_DIR}/dependent_projects/dependencies.txt" )
	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )
endif( )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )
#include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
#include_directories( SYSTEM ${CURL_INCLUDE_DIRS} )

link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FILES
	${HEADER_FOLDER}/daw/glean/action_status.h
	${HEADER_FOLDER}/daw/glean/build_cmake.h
	${HEADER_FOLDER}/daw/glean/build_none.h
	${HEADER_FOLDER}/daw/glean/build_types.h
	${HEADER_FOLDER}/daw/glean/cmake_helper.h
	${HEADER_FOLDER}/daw/glean/dependency.h
	${HEADER_FOLDER}/daw/glean/download_git.h
	${HEADER_FOLDER}/daw/glean/download_none.h
	${HEADER_FOLDER}/daw/glean/download_svn.h
	${HEADER_FOLDER}/daw/glean/download_types.h
	${HEADER_FOLDER}/daw/glean/git_helper.h
	${HEADER_FOLDER}/daw/glean/glean_config.h
	${HEADER_FOLDER}/daw/glean/glean_file.h
	${HEADER_FOLDER}/daw/glean/glean_file_item.h
	${HEADER_FOLDER}/daw/glean/glean_options.h
	${HEADER_FOLDER}/daw/glean/logging.h
	${HEADER_FOLDER}/daw/glean/proc.h
	${HEADER_FOLDER}/daw/glean/svn_helper.h
	${HEADER_FOLDER}/daw/glean/utilities.h
	${HEADER_FOLDER}/daw/glean/impl/build_types_impl.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/build_cmake.cpp
	${SOURCE_FOLDER}/cmake_helper.cpp
	${SOURCE_FOLDER}/dependency.cpp
	${SOURCE_FOLDER}/download_git.cpp
	${SOURCE_FOLDER}/download_svn.cpp
	${SOURCE_FOLDER}/git_helper.cpp
	${SOURCE_FOLDER}/glean_config.cpp
	${SOURCE_FOLDER}/glean_file.cpp
	${SOURCE_FOLDER}/glean_options.cpp
	${SOURCE_FOLDER}/logging.cpp
	${SOURCE_FOLDER}/svn_helper.cpp
	${SOURCE_FOLDER}/glean_file.cpp
)

add_executable( glean ${HEADER_FILES} ${SOURCE_FILES} ${SOURCE_FOLDER}/glean.cpp )
#target_link_libraries( glean utf_range temp_file ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )
target_link_libraries( glean utf_range temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
add_dependencies( glean dependency_stub )

install( TARGETS glean DESTINATION bin )


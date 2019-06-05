cmake_minimum_required( VERSION 3.0.2 )

project( glean_prj )

find_package( Boost 1.58.0 COMPONENTS system iostreams program_options filesystem regex unit_test_framework REQUIRED )
find_package( Threads REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( CURL REQUIRED )

enable_testing( )

add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_NETWORK_ENABLE_HTTPS -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )

if( EXISTS "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${CMAKE_SOURCE_DIR}/dependent_projects/build/install/include" )
	link_directories( "${CMAKE_SOURCE_DIR}/dependent_projects/build/install/lib" )
else( )
	set( SHOULD_BUILD_DEPS ON )
	include( "${CMAKE_SOURCE_DIR}/dependent_projects/dependencies.txt" )

	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
    add_dependencies( dependency_stub temp_file_prj date_prj header_libraries_prj utf_range_prj iso8601_parsing_prj daw_json_link_prj )
	include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
endif( )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )
include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( SYSTEM ${CURL_INCLUDE_DIRS} )

link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FILES
	${HEADER_FOLDER}/action_status.h
	${HEADER_FOLDER}/build_cmake.h
	${HEADER_FOLDER}/build_none.h
	${HEADER_FOLDER}/build_types.h
	${HEADER_FOLDER}/dependency.h
	${HEADER_FOLDER}/glean_config.h
	${HEADER_FOLDER}/git_helper.h
	${HEADER_FOLDER}/download_git.h
	${HEADER_FOLDER}/download_none.h
	${HEADER_FOLDER}/download_types.h
	${HEADER_FOLDER}/glean_file.h
	${HEADER_FOLDER}/glean_options.h
	${HEADER_FOLDER}/utilities.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/download_git.cpp
	${SOURCE_FOLDER}/glean_config.cpp
	${SOURCE_FOLDER}/git_helper.cpp
	${SOURCE_FOLDER}/glean_file.cpp
	${SOURCE_FOLDER}/glean_options.cpp
	${SOURCE_FOLDER}/utilities.cpp
)

add_executable( glean ${HEADER_FILES} ${SOURCE_FILES} ${SOURCE_FOLDER}/glean.cpp )
target_link_libraries( glean utf_range git2 gnutls tz temp_file ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )
add_dependencies( glean dependency_stub )

add_executable( glean_test_bin ${HEADER_FILES} ${SOURCE_FILES} ${TEST_FOLDER}/glean_tests.cpp )
add_test( glean_test glean_test_bin )
target_link_libraries( glean_test_bin git2 gnutls tz utf_range temp_file ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )
add_dependencies( glean_test_bin dependency_stub )
#add_test( glean_test glean_test_bin )

install( TARGETS glean DESTINATION bin )


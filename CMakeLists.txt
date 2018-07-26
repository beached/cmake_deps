cmake_minimum_required( VERSION 2.8.12 )

project( glean_prj )

find_package( Boost 1.58.0 COMPONENTS system iostreams program_options filesystem regex unit_test_framework REQUIRED )
find_package( Threads REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( CURL REQUIRED )


enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_NETWORK_ENABLE_HTTPS -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 /std:c++latest )
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
		add_compile_options(-std=c++14 -Weverything -pthread -Wno-c++98-compat -g -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -g -Wall -pthread -Wno-deprecated-declarations)
	endif( )
endif( )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

if( EXISTS "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${CMAKE_SOURCE_DIR}/dependent_projects/build/install/include" )
	link_directories( "${CMAKE_SOURCE_DIR}/dependent_projects/build/install/lib" )
else( )
	include( "${CMAKE_SOURCE_DIR}/dependent_projects/dependencies.txt" )
endif( )

include_directories( ${HEADER_FOLDER} )
include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( SYSTEM ${CURL_INCLUDE_DIRS} )

link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FILES
	${HEADER_FOLDER}/config.h
	${HEADER_FOLDER}/git_helper.h
	${HEADER_FOLDER}/glean_file.h
	${HEADER_FOLDER}/glean_file_parser.h
	${HEADER_FOLDER}/glean_impl.h
	${HEADER_FOLDER}/glean_options.h
	${HEADER_FOLDER}/templates.h
	${HEADER_FOLDER}/utilities.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/config.cpp
	${SOURCE_FOLDER}/git_helper.cpp
	${SOURCE_FOLDER}/glean_file.cpp
	${SOURCE_FOLDER}/glean_file_parser.cpp
	${SOURCE_FOLDER}/glean_impl.cpp
	${SOURCE_FOLDER}/glean_options.cpp
	${SOURCE_FOLDER}/templates.cpp
	${SOURCE_FOLDER}/utilities.cpp
)

add_executable( glean ${HEADER_FILES} ${SOURCE_FILES} ${SOURCE_FOLDER}/glean.cpp )
target_link_libraries( glean utf_range parse_template git2 gnutls kv_file tz temp_file ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )

add_executable( glean_test_bin ${HEADER_FILES} ${SOURCE_FILES} ${TEST_FOLDER}/glean_tests.cpp )
add_test( glean_test glean_test_bin )
target_link_libraries( glean_test_bin kv_file parse_template git2 gnutls tz utf_range temp_file ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )
#add_test( glean_test glean_test_bin )

install( TARGETS glean DESTINATION bin )


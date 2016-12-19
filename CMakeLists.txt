cmake_minimum_required( VERSION 2.8.12 )

project( glean_prj )

include( ExternalProject )

find_package( Boost 1.58.0 COMPONENTS system iostreams program_options filesystem regex unit_test_framework REQUIRED )

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_NETWORK_ENABLE_HTTPS ) 

find_package( Threads REQUIRED )

find_package( OpenSSL REQUIRED )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
		add_compile_options(-std=c++14 -Weverything -pthread -Wno-c++98-compat -g -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -g -Wall -pthread -Wno-deprecated-declarations)
	endif( )
endif( )

externalproject_add(
    header_libraries_prj
    GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/header_libraries"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
    kv_file_prj
    GIT_REPOSITORY "https://github.com/beached/kv_file.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/kv_file"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
    char_range_prj
    GIT_REPOSITORY "https://github.com/beached/char_range.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/char_range"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
    parse_template_prj
    GIT_REPOSITORY "https://github.com/beached/parse_template.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/parse_template"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
    temp_file_prj
    GIT_REPOSITORY "https://github.com/beached/libtemp_file.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/temp_file"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
include_directories( ${HEADER_FOLDER} )
include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( SYSTEM ${CURL_INCLUDE_DIRS} )

link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FILES
	${HEADER_FOLDER}/config.h
	${HEADER_FOLDER}/glean_file.h
	${HEADER_FOLDER}/glean_file_parser.h
	${HEADER_FOLDER}/glean_impl.h
	${HEADER_FOLDER}/templates.h
	${HEADER_FOLDER}/utilities.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/glean_file.cpp
	${SOURCE_FOLDER}/glean_file_parser.cpp
	${SOURCE_FOLDER}/glean_impl.cpp
	${SOURCE_FOLDER}/config.cpp
	${SOURCE_FOLDER}/templates.cpp
	${SOURCE_FOLDER}/utilities.cpp
)

add_executable( glean ${HEADER_FILES} ${SOURCE_FILES} ${SOURCE_FOLDER}/glean.cpp )
add_dependencies( glean header_libraries_prj char_range_prj parse_template_prj kv_file_prj temp_file_prj )
target_link_libraries( glean char_range parse_template kv_file temp_file ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )

add_executable( glean_test_bin ${HEADER_FILES} ${SOURCE_FILES} ${TEST_FOLDER}/glean_tests.cpp )
add_test( glean_test glean_test_bin )
add_dependencies( glean_test_bin header_libraries_prj char_range_prj parse_template_prj kv_file_prj temp_file_prj )
target_link_libraries( glean_test_bin kv_file parse_template char_range temp_file ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )
#add_test( glean_test glean_test_bin )

install( TARGETS glean DESTINATION bin )

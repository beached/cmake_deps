cmake_minimum_required( VERSION 3.0.2 )

project( glean_prj )

#find_package( Boost 1.58.0 COMPONENTS system iostreams program_options filesystem regex unit_test_framework REQUIRED )
find_package( Boost 1.62.0 COMPONENTS program_options filesystem REQUIRED )
#find_package( Threads REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( CURL REQUIRED )

enable_testing( )

#add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_NETWORK_ENABLE_HTTPS -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

set( DEP_ROOT_DIR_BASE "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/debug" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
endif( )

message( "Building for: ${CMAKE_BUILD_TYPE}" )
if( EXISTS "${DEP_ROOT_DIR}" )
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
target_link_libraries( glean utf_range temp_file git2 ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )
add_dependencies( glean dependency_stub )

#add_executable( glean_test_bin ${HEADER_FILES} ${SOURCE_FILES} ${TEST_FOLDER}/glean_tests.cpp )
#add_test( glean_test glean_test_bin )
#target_link_libraries( glean_test_bin gnutls tz utf_range temp_file ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} )
#target_link_libraries( glean_test_bin utf_range temp_file ${Boost_LIBRARIES} ) 
#add_dependencies( glean_test_bin dependency_stub )
#add_test( glean_test glean_test_bin )

add_executable( git_test ${SOURCE_FOLDER}/git_helper.cpp ${TEST_FOLDER}/git_test.cpp )
target_link_libraries( git_test git2 ${Boost_LIBRARIES} )
add_dependencies( git_test dependency_stub )

install( TARGETS glean DESTINATION bin )


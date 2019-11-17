cmake_minimum_required( VERSION 2.8.12 )

project( dependency_prj )

add_definitions( -DBOOST_ALL_NO_LIB )
find_package( Boost 1.62.0 COMPONENTS program_options system iostreams filesystem REQUIRED)
find_package( OpenSSL REQUIRED )
find_package( Threads REQUIRED )

set( CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")

include( "CMakeListsCompiler.txt" )

include_directories( "./include" )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

include( "dependencies.txt" )

add_library( dependency_stub STATIC stub.cpp )
add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )


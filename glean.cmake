include( ExternalProject )
externalproject_add(
  date_prj
  GIT_REPOSITORY "https://github.com/beached/date.git"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/date"
  INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DGLEAN_INSTALL_ROOT=${CMAKE_BINARY_DIR}/install -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}  -DUSE_SYSTEM_TZ_DB=true -DENABLE_DATE_TESTING=OFF
)

externalproject_add(
  iso8601_parsing_prj
  GIT_REPOSITORY "https://github.com/beached/iso8601_parsing.git"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/iso8601_parsing"
  INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DGLEAN_INSTALL_ROOT=${CMAKE_BINARY_DIR}/install -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
)

externalproject_add(
  header_libraries_prj
  GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/header_libraries"
  INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DGLEAN_INSTALL_ROOT=${CMAKE_BINARY_DIR}/install -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
)

externalproject_add(
  daw_json_link_prj
  DEPENDS date_prj iso8601_parsing_prj header_libraries_prj
  GIT_REPOSITORY "https://github.com/beached/daw_json_link"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/daw_json_link"
  INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DGLEAN_INSTALL_ROOT=${CMAKE_BINARY_DIR}/install -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
)

externalproject_add(
  fmtlib_prj
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/fmtlib"
  INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DGLEAN_INSTALL_ROOT=${CMAKE_BINARY_DIR}/install -DFMT_TEST=off -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
)

externalproject_add(
  utf_range_prj
  DEPENDS header_libraries_prj
  GIT_REPOSITORY "https://github.com/beached/utf_range"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/utf_range"
  INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DGLEAN_INSTALL_ROOT=${CMAKE_BINARY_DIR}/install -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
set( DEP_PROJECT_DEPS fmtlib_prj utf_range_prj date_prj iso8601_parsing_prj daw_json_link_prj header_libraries_prj )


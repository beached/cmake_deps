rename glean.cmake.renamewin glean.cmake
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_STATIC_BOOST=ON -DBOOST_ROOT="%BOOST_ROOT%" -DCMAKE_TOOL_CHAIN_FILE="%VCPKG_INSTALLATION_ROOT%/scripts/buildsystems/vcpkg.cmake" ..
cmake --build . --config Release --target -j 2

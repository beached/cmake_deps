mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT="%BOOST_ROOT%" -DCMAKE_TOOL_CHAIN_FILE="%VCPKG_INSTALLATION_ROOT%/scripts/buildsystems/vcpkg.cmake" ..
cmake --build . --config Release --target -j 2

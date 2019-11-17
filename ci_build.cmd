
md build
cd build
pwd
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOL_CHAIN_FILE="%VCPKG_INSTALLATION_ROOT%/scripts/buildsystems/vcpkg.cmake" ..
cmake --build . --config Release --target full -j 2

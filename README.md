# Glean
Uses cmake's external project as a facility to install project dependencies into a top level location with central caching

By putting a glean.txt file at the top of the project directory and filling it in as follows.  Currently only git repos are supported but more soon

glean.txt

```
project_name=Glean
    type=git
    uri=https://github.com/beached/glean.git
    branch=master
```

If branch is omitted it defaults to master.  The value for project_name must be a valid part of a filename and cannot use characters not allowed in a filename

From the same directory as glean.txt run glean and it will create a folder called glean_files.  Commonly one would add something like the following to a CMakeLists.txt to make the headers and libraries available.

```
link_directories( "${CMAKE_SOURCE_DIR}/glean_files/lib" )
include_directories( SYSTEM "${CMAKE_SOURCE_DIR}/glean_files/include" )
```

By default a config file in the home folder( %USERPROFILE% on Windows and $HOME on Linux/Mac...) is creted with the name .glean.config.  It has two parameters, currently, called cache_folder and cmake_binary.  The default file is as follows:
```
cache_folder=$HOME/.glean_cache
cmake_binary=cmake
```
Where home will be the expanded $HOME or %USERPROFILE% environment variables.  Unknown keys are ignored but cause a warning.

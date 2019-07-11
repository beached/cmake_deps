# Glean
## Dependency management, with a focus on source

Define your projects dependencies in a simple JSON file.  Currently, git and cmake are supported with partial svn support.  

A project an express their dependencies in a `glean.json` file in the root of their project.  It has a format like

```JSON
{
	"provides": "glean",
	"build_type": "cmake",
	"dependencies": [ 
		{
			"name": "header_libraries",
			"download_type": "git",
			"build_type": "cmake",
			"uri": "https://github.com/beached/header_libraries",
			"version": "tag_branch_or_revision"
		},
		{
			"name": "libtemp_file",
			"download_type": "git",
			"build_type": "cmake",
			"uri": "https://github.com/beached/libtemp_file"
		},
		{
			"name": "daw_json_link",
			"download_type": "git",
			"build_type": "cmake",
			"uri": "https://github.com/beached/daw_json_link"
		},
		{
			"name": "utf_range",
			"download_type": "git",
			"build_type": "cmake",
			"uri": "https://github.com/beached/utf_range"
		}
	]
}
```
This will dowload each of the dependencies and recursively scan for a glean.json file.  Currently, duplicates are not supported and take the first one seen.

The inside a cmake project one can put something along the lines of 
```
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/debug" )
else( )
	set( GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/release" )
endif( )
include_directories( SYSTEM "${GLEAN_CACHE}/include" )
link_directories( "${GLEAN_CACHE}/lib" )

```

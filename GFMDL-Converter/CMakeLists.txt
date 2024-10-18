# Version
set(GFMDL_CONV_VERSION_MAJOR 0)
set(GFMDL_CONV_VERSION_MINOR 0)
set(GFMDL_CONV_VERSION_PATCH 1)

# Libraries
set(GFMDL_CONV_LIBRARIES_ROOT "${CMAKE_SOURCE_DIR}/GFMDL-Converter/vendor")

# Project version
configure_file("${CMAKE_SOURCE_DIR}/GFMDL-Converter/src/Version.hpp.in" "${CMAKE_SOURCE_DIR}/GFMDL-Converter/src/Version.hpp")

# Source files
file(GLOB_RECURSE GFMDL_CONV_HEADERS src/*.hpp)
file(GLOB_RECURSE GFMDL_CONV_SOURCES src/*.cpp)

add_executable(GFMDL-Converter ${GFMDL_CONV_HEADERS} ${GFMDL_CONV_SOURCES})
target_precompile_headers(GFMDL-Converter PRIVATE ${CMAKE_SOURCE_DIR}/GFMDL-Converter/src/Pch.hpp)
target_include_directories(GFMDL-Converter PRIVATE src)

# Options
option(GFMDL_CONV_SKIP_OUTPUT "Don't output any file, useful for debug" OFF)

if(GFMDL_CONV_SKIP_OUTPUT)
	target_compile_definitions(GFMDL-Converter PRIVATE GFMDL_CONV_SKIP_OUTPUT)
endif()

# pugixml
set(PUGIXML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PUGIXML_INSTALL OFF CACHE BOOL "" FORCE)
target_include_directories(GFMDL-Converter PRIVATE ${NESEMU_LIBRARIES_ROOT}/src)
add_subdirectory(${GFMDL_CONV_LIBRARIES_ROOT}/pugixml)
target_link_libraries(GFMDL-Converter PRIVATE pugixml-static)
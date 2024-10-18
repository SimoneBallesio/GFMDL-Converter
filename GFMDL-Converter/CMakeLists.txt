# Libraries
set(GFMDL_CONV_LIBRARIES_ROOT "${CMAKE_SOURCE_DIR}/GFMDL-Converter/vendor")

# Source files
file(GLOB_RECURSE GFMDL_CONV_HEADERS src/*.hpp)
file(GLOB_RECURSE GFMDL_CONV_SOURCES src/*.cpp)

add_executable(GFMDL-Converter ${GFMDL_CONV_HEADERS} ${GFMDL_CONV_SOURCES})
target_precompile_headers(GFMDL-Converter PRIVATE ${CMAKE_SOURCE_DIR}/GFMDL-Converter/src/Pch.hpp)
target_include_directories(GFMDL-Converter PRIVATE src)

# pugixml
set(PUGIXML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PUGIXML_INSTALL OFF CACHE BOOL "" FORCE)
target_include_directories(GFMDL-Converter PRIVATE ${NESEMU_LIBRARIES_ROOT}/src)
add_subdirectory(${NESEMU_LIBRARIES_ROOT}/pugixml)
target_link_libraries(GFMDL-Converter PRIVATE pugixml-static)
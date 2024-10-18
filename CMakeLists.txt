cmake_minimum_required(VERSION 3.5)
project(GFMDL-Converter)

# Global configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DGFMDL_CONV_DEBUG -m64 -Wall -fvisibility=hidden")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -m64 -Wall -O3 -fvisibility=hidden")

# Platform-specific global configuration
if(WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mwindows")
endif()

# Projects
add_subdirectory("${CMAKE_SOURCE_DIR}/GFMDL-Converter")
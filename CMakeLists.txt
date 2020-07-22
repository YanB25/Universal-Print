cmake_minimum_required(VERSION 3.12.4)

project (helloworld)

option(BUILD_BINARIES "Build the binaries in {project}/bin directory" ON)
option(BUILD_STATIC_LIBS "Build the static library" ON)
option(BUILD_SHARED_LIBS "Build the shared library" ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(HELLOWORLD_INCLUDE_DIRECTORIES include)

file(GLOB_RECURSE HELLOWORLD_SOURCE src/*)
add_library(helloworld_obj OBJECT ${HELLOWORLD_SOURCE})
target_include_directories(helloworld_obj PUBLIC ${HELLOWORLD_INCLUDE_DIRECTORIES})

if (${BUILD_BINARIES})
    add_subdirectory(bin)
endif()

if (${BUILD_STATIC_LIBS})
    add_library(helloworld-static STATIC $<TARGET_OBJECTS:helloworld_obj>)
endif()

if (${BUILD_SHARED_LIBS})
    add_library(helloworld-dynamic SHARED $<TARGET_OBJECTS:helloworld_obj>)
endif()
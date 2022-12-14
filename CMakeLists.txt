cmake_minimum_required(VERSION 3.12.4)

project (helloworld)

option(BUILD_BINARIES "Build the binaries in {project}/bin directory" ON)
option(ENABLE_GTEST "Enable gtest" OFF)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# dir to include
set(HELLOWORLD_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include)

add_compile_options("-Wall" "-W" "-Wextra" "-fPIC")

# headers to install
file(GLOB_RECURSE HELLOWORLD_PUBLIC_HEADER include/*)

if (${BUILD_BINARIES})
    add_subdirectory(bin)
endif()

if (${ENABLE_GTEST})
    add_subdirectory(third_party/gtest)
    include_directories(third_party/gtest/googletest/include)
    add_subdirectory(unittest)
    enable_testing()
endif()
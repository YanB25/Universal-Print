cmake_minimum_required(VERSION 3.12.4)

project (helloworld)

option(BUILD_BINARIES "Build the binaries in {project}/bin directory" ON)
option(BUILD_UNITTEST "Build the unittest in {project}/test directory" ON)
option(BUILD_STATIC_LIBS "Build the static library" ON)
option(BUILD_SHARED_LIBS "Build the shared library" ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# dir to include
set(HELLOWORLD_INCLUDE_DIRECTORIES include)

add_compile_options("-Wall" "-W" "-Wextra" "-fPIC")

add_subdirectory(gtest)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# headers to install
file(GLOB_RECURSE HELLOWORLD_PUBLIC_HEADER include/*)

# source for the lib
file(GLOB_RECURSE HELLOWORLD_SOURCE src/*)
add_library(helloworld_obj OBJECT ${HELLOWORLD_SOURCE})
target_include_directories(helloworld_obj PUBLIC ${HELLOWORLD_INCLUDE_DIRECTORIES})

if (${BUILD_UNITTEST})
    add_subdirectory(test)
    enable_testing()
endif()

if (${BUILD_BINARIES})
    add_subdirectory(bin)
endif()

if (${BUILD_STATIC_LIBS})
    add_library(helloworld-static STATIC $<TARGET_OBJECTS:helloworld_obj>)

    # install instructions
    set_target_properties(helloworld-static PROPERTIES PUBLIC_HEADER "${HELLOWORLD_PUBLIC_HEADER}")
    install(TARGETS helloworld-static PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/helloworld
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()

if (${BUILD_SHARED_LIBS})
    add_library(helloworld-dynamic SHARED $<TARGET_OBJECTS:helloworld_obj>)

    # install instructions
    set_target_properties(helloworld-dynamic PROPERTIES PUBLIC_HEADER "${HELLOWORLD_PUBLIC_HEADER}")
    install(TARGETS helloworld-dynamic PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/helloworld
        LIBRARY DESTINATION lib)
endif()

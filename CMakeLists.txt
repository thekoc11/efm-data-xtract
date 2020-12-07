cmake_minimum_required(VERSION 3.16)
project(proj)


set(CMAKE_CXX_STANDARD 17)
add_subdirectory("lib/FFmpeg")

FIND_PACKAGE(MPI REQUIRED)

if (MPI_FOUND)
    message("MPI VERSION ${MPI_VERSION} found")
else()
    message("MPI NOT FOUND!")
endif ()

set(PROJECT_INCLUDE_PATH "./inc")

#add_executable(proj  src/main.c)# src/filter.cpp src/filter.hpp src/main.cpp)
add_executable(proj  src/filter.cpp src/main.cpp src/fileops.cpp)

target_include_directories(proj PRIVATE ${MPI_CXX_INCLUDE_PATH} ${PROJECT_INCLUDE_PATH})
target_compile_options(proj PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(proj ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS} FFmpeg pthread)
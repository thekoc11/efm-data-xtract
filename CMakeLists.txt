cmake_minimum_required(VERSION 3.16)
project(proj)


set(CMAKE_CXX_STANDARD 17)
add_subdirectory("lib/FFmpeg")

#FIND_PACKAGE(MPI REQUIRED)
#FIND_PACKAGE(x264 REQUIRED)
#if (x264_FOUND)
#    message("x264 VERSION ${x264_VERSION} found")
#else()
#    message("x264 NOT FOUND!")
#endif ()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++17 -stdlib=libc++")

set(PROJECT_INCLUDE_PATH "./inc")

#add_executable(proj  src/main.c)# src/filter.cpp src/filter.hpp src/main.cpp)
add_executable(proj  src/filter.cpp src/main.cpp src/fileops.cpp inc/Raga.h src/Raga.cpp src/stringops.cpp inc/stringops.h src/notationops.cpp inc/notationops.h)

target_include_directories(proj PRIVATE ${MPI_CXX_INCLUDE_PATH} ${PROJECT_INCLUDE_PATH})
target_compile_options(proj PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(proj x264 ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS} FFmpeg pthread)
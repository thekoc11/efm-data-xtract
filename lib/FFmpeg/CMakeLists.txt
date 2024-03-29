cmake_minimum_required(VERSION 3.16)
project(FFmpeg)
set(ENV{PKG_CONFIG_PATH} "/home/$USER/libav/lib/pkgconfig")

find_package(PkgConfig REQUIRED)
pkg_search_module(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVFILTER REQUIRED IMPORTED_TARGET libavfilter)
pkg_check_modules(AVDEVICE REQUIRED IMPORTED_TARGET libavdevice)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(SWRESAMPLE REQUIRED IMPORTED_TARGET libswresample)
pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)


add_library(FFmpeg INTERFACE IMPORTED GLOBAL)
target_include_directories(FFmpeg INTERFACE
        ${AVCODEC_INCLUDE_DIRS}
        ${AVFORMAT_INCLUDE_DIRS}
        ${AVFILTER_INCLUDE_DIRS}
        ${AVDEVICE_INCLUDE_DIRS}
        ${AVUTIL_INCLUDE_DIRS}
        ${SWRESAMPLE_INCLUDE_DIRS}
        ${SWSCALE_INCLUDE_DIRS}
        )
message("Libraries' include dir: ${AVFORMAT_INCLUDE_DIRS}, ${AVCODEC_LDFLAGS}")
target_link_libraries(FFmpeg INTERFACE
        ${AVCODEC_LDFLAGS}
        ${AVFORMAT_LDFLAGS}
        ${AVFILTER_LDFLAGS}
        ${AVDEVICE_LDFLAGS}
        ${AVUTIL_LDFLAGS}
        ${SWRESAMPLE_LDFLAGS}
        ${SWSCALE_LDFLAGS}
        )

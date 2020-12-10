cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)
PROJECT(AikitDemo)

set(OpenCV_DIR "/usr/local/opencv/share/OpenCV/")

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)


set(GSTREAMER_INCLUDE_DIRS "/usr/include/gstreamer-1.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include/glib-2.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include/glib-2.0/include;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include/gobject;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include/gst;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/lib/gstreamer-1.0/include;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/include/glib-2.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/lib/glib-2.0/include;")
set(GSTREAMER_LIBS  "gstapp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstbase-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstrtsp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstreamer-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstsdp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gio-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gobject-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "glib-2.0;")
set(GSTREAMER_LIBRARY_DIRS  "/usr/lib/;")

#YUV
set(YUV_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include;")
set(YUV_LIBS  "yuv;")
set(YUV_LIBRARY_DIRS  "${CMAKE_SOURCE_DIR}/lib;")
set(YUV_STATIC_LIBS  "${CMAKE_SOURCE_DIR}/lib/libyuv.a;")


#config
set(PARAM_CONF_LIBS "config")
set(PARAM_CONF_LIBS_DIRS "${CMAKE_SOURCE_DIR}//lib//;")
set(PARAM_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/config/include/;")


set(MULTPLAYER_INC_DIRS "${CMAKE_SOURCE_DIR}/include/;")
set(MULTPLAYER_LIB_DIRS  "${CMAKE_SOURCE_DIR}/lib/;")
set(MultPlayer "MultPlayer;") 

set(OPENCV33_LIBS  "opencv_core;opencv_imgproc;")
set(COMMON_LIBRARY_DIRS  "${CMAKE_SOURCE_DIR}/lib/;")

message(STATUS "    GST DEBUG:###########################")
message(STATUS "    libraries: ${GSTREAMER_LIBS}")
message(STATUS "    include path: ${GSTREAMER_INCLUDE_DIRS}")
message(STATUS "    library path: ${GSTREAMER_LIBRARY_DIRS}")


#find_package( REQUIRED )

find_library( # Sets the name of the path variable.
              log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

include_directories(${CMAKE_SOURCE_DIR}/
                   ${OpenCV_INCLUDE_DIRS}/
                   ${GSTREAMER_INCLUDE_DIRS}/
                   ${GSTREAMER_APP_INCLUDE_DIRS}/
                   ${GSTREAMER_RTSP_INCLUDE_DIRS}/
                   ${YUV_INCLUDE_DIRS}/
				   ${MULTPLAYER_INC_DIRS}/
                   ${PARAM_INCLUDE_DIRS}/
                   ${CMAKE_SOURCE_DIR}/include )


link_directories(${OpenCV_LIBRARY_DIRS}/
                   ${GSTREAMER_LIBRARY_DIRS}/
                   ${YUV_LIBRARY_DIRS}/
				   ${MULTPLAYER_LIB_DIRS}/
                   ${PARAM_CONF_LIBS_DIRS}
		)


ADD_SUBDIRECTORY(yuv)
aux_source_directory(config/src CONFIG_SRC)
aux_source_directory(MultPlayer MultPlayer_SRC)

add_executable(multi-decoder MultiDecoder.cpp ${CONFIG_SRC} ${MultPlayer_SRC})

# opencv libraries
target_link_libraries(multi-decoder  ${OpenCV_LIBS} ${GSTREAMER_LIBS} ${YUV_STATIC_LIBS})

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/)

set(CMAKE_CXX_FLAGS "-std=c++11  -lm -pthread")

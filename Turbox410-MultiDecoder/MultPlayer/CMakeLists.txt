#1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 3.2) 
set(CMAKE_VERBOSE_MAKEFILE on)
#2.project name，指定项目的名称，一般和项目的文件夹名称对应 
PROJECT(mpreivew_app)

set(LIB_NAME "MultPlayer") 

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
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/)


set(SRC_DIR  "./")
set(GSTREAMER_INCLUDE_DIRS "/usr/include/gstreamer-1.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/lib//gstreamer-1.0/include;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/include/glib-2.0;")
set(GSTREAMER_INCLUDE_DIRS ${GSTREAMER_INCLUDE_DIRS} "/usr/lib//glib-2.0/include;")
set(GSTREAMER_LIBS  "gstapp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstbase-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstrtsp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstreamer-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gstsdp-1.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gio-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "gobject-2.0;")
set(GSTREAMER_LIBS ${GSTREAMER_LIBS} "glib-2.0;")
set(GSTREAMER_LIBRARY_DIRS  "/usr/lib/;")

set(GLIB_INC "${CMAKE_SOURCE_DIR}/glib-2.0/")


#OPENGL
set(OPENGL_LIBS  "GL;GLU;glut;GLEW;")

message(STATUS "    GST DEBUG:###########################")
message(STATUS "    libraries: ${GSTREAMER_LIBS}")
message(STATUS "    include path: ${GSTREAMER_INCLUDE_DIRS}")
message(STATUS "    library path: ${GSTREAMER_LIBRARY_DIRS}")


set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


set(SRC_DIR "${SRC_DIR}/")
set(EXT_INC_DIR "${SRC_DIR}/include/")
set(INC_DIR "${SRC_DIR}/")
set(EXT_LIB_DIRS "${SRC_DIR}/lib/")
set(PARENT_INC "${SRC_DIR}/../")
#3.head file path，头文件目录
INCLUDE_DIRECTORIES(${SRC_DIR}/
                   ${OpenCV_INCLUDE_DIRS}/
				   ${EXT_INC_DIR}/
				   ${PARENT_INC}/
				   ${GLIB_INC}/
				   ${INC_DIR}
	)


LINK_DIRECTORIES( ${OpenCV_LIBRARY_DIRS}/
                   ${GSTREAMER_LIBRARY_DIRS}/
				   ${EXT_LIB_DIRS})

aux_source_directory(${SRC_DIR} DIR_SRCS)

#5.set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
SET(TEST_MATH ${DIR_SRCS}
	)

file(GLOB my_src ${SRC_DIR}/*.c  ${SRC_DIR}/*.cpp)

#6.add executable file添加要编译的可执行文件
#add_executable(ai_client ${my_src})

#add_library(${LIB_NAME} SHARED ${DIR_SRCS} )
add_library(${LIB_NAME} SHARED ${DIR_SRCS} )

#7.add link library，添加可执行文件所需要的库，比如我们用到了libm.so（命名规则：lib+name+.so），就添加该库的名称
target_link_libraries(MultPlayer  ${OpenCV_LIBS}  ${GSTREAMER_LIBS} "-lm -lpthread")

set(CMAKE_CXX_FLAGS "-std=c++0x  -lm -lpthread")
set(CMAKE_C_FLAGS " -lm -lpthread")


cmake_minimum_required(VERSION 3.9)

SET(CMAKE_C_COMPILER "/data/impala/toolchain/gcc-4.9.2/bin/gcc")
SET(CMAKE_CXX_COMPILER "/data/impala/toolchain/gcc-4.9.2/bin/g++")
set(CMAKE_CXX_STANDARD 11)

project(Person_Learn)

#SET(CMAKE_C_COMPILER "/data/impala/toolchain/gcc-4.9.2/bin/gcc")
#SET(CMAKE_CXX_COMPILER "/data/impala/toolchain/gcc-4.9.2/bin/g++")

set(BOOST_ROOT "/data/impala/toolchain/boost-1.57.0-p3")
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)
set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)

message("Hello, start compile!!!")
message(STATUS "Boost include dir: " ${BOOST_INCLUDEDIR})
message(STATUS "Boost libraries: " ${BOOST_LIBRARYDIR})

include_directories("${PROJECT_SOURCE_DIR}/tuple")
include_directories("${PROJECT_SOURCE_DIR}/util")
#include_directories("${PROJECT_SOURCE_DIR}/pattern")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories(${BOOST_INCLUDEDIR})

add_subdirectory(tuple)
add_subdirectory(util)
add_subdirectory(prometheus)
add_subdirectory(pattern)
add_subdirectory(mutex)

aux_source_directory(./ DIR_SRCS) #将当前目录下的所有源文件保存在变量DIR_SRCS中
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

link_directories(${BOOST_LIBRARYDIR})
add_executable(person_learn ${DIR_SRCS}) #根据源文件生成可执行文件person_learn

target_link_libraries(person_learn tuple util pattern mutex pthread PrometheusCore boost_filesystem boost_system boost_thread boost_log boost_program_options)

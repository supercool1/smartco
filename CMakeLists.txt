cmake_minimum_required(VERSION 3.5)

project(smartco)
# set(LIBRARY_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/install/bin)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/install/lib)

if (NOT CMAKE_BUILD_TYPE)
   set (CMAKE_BUILD_TYPE "Release") #"Release" "Debug"
   message (
      STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}"
   )
endif ()
OPTION (TEXT                  "Build tests"                            ON)



set (CMAKE_CXX_FLAGS "-fPIC -std=c++11 -Wall -Werror")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -o3")

include_directories(${CMAKE_SOURCE_DIR}/smartco)
MESSAGE(${CMAKE_SOURCE_DIR}/smartco)

file(GLOB SOURCE_FILE  ${CMAKE_SOURCE_DIR}/smartco/*.cpp)

add_library(smartco SHARED ${SOURCE_FILE})
target_link_libraries(smartco   -pthread )

if(TEXT)
   add_executable(test_ ${CMAKE_SOURCE_DIR}/test/test_thread.cpp)
   target_link_libraries(test_  smartco -pthread)
endif()
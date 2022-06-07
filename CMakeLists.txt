cmake_minimum_required(VERSION 3.5)

project(smartco)
# set(LIBRARY_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/install/bin)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/install/lib)

if (NOT CMAKE_BUILD_TYPE)
   set (CMAKE_BUILD_TYPE "Debug") #"Release" "Debug"
   message (
      STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}"
   )
endif ()
OPTION (TEXT                  "Build tests"                            ON)



set (CMAKE_CXX_FLAGS "-fPIC -std=c++11 -Wall")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -o3")

include_directories(${CMAKE_SOURCE_DIR}/smartco)
MESSAGE(${CMAKE_SOURCE_DIR}/smartco)

file(GLOB SOURCE_FILE  ${CMAKE_SOURCE_DIR}/smartco/*.cc)

add_library(smartco SHARED ${SOURCE_FILE})
target_link_libraries(smartco   -pthread )

if(TEXT)
   add_executable(test_thread ${CMAKE_SOURCE_DIR}/test/test_thread.cc)
   target_link_libraries(test_thread  smartco -pthread)

   add_executable(test_fiber ${CMAKE_SOURCE_DIR}/test/test_fiber.cc)
   target_link_libraries(test_fiber  smartco -pthread)

   add_executable(test_scheduler ${CMAKE_SOURCE_DIR}/test/test_scheduler.cc)
   target_link_libraries(test_scheduler  smartco -pthread)

endif()
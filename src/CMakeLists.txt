cmake_minimum_required(VERSION 2.8.8)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "In-source builds are not allowed.
Please create a directory and run cmake from there, passing the path to this source directory as the last argument.
This process created the file `CMakeCache.txt' and the directory `CMakeFiles'. Please delete them.")
endif()

project(sentry-cpp CXX)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# TODO: Need to explicitly include pthread due to a bug in netlib(0.12)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g  -pthread")

find_package (cppnetlib 0.11.0 REQUIRED)
include_directories (
    SYSTEM
    ${CPPNETLIB_INCLUDE_DIRS}
    ../include
    )

ADD_LIBRARY(sentryclient STATIC
    SentryClient.cpp
    )

target_link_libraries(sentryclient ${CPPNETLIB_LIBRARIES})

install (TARGETS sentryclient DESTINATION lib)
install (FILES ../include/SentryClient.h ../include/SentryMessage.h DESTINATION include/sentryclient)

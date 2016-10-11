set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -pthread")

include_directories(../include)

add_executable (testSentry test.cpp)

target_link_libraries (testSentry LINK_PUBLIC sentryclient)

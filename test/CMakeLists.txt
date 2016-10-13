set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -pthread")

add_executable (testSentry test.cpp)

target_link_libraries (testSentry LINK_PUBLIC sentryclient)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -pthread")

find_package (cppnetlib 0.11.0 REQUIRED)
include_directories (
    SYSTEM
    ${CPPNETLIB_INCLUDE_DIRS}
    ../include
    )

add_executable (testSentry test.cpp)

target_link_libraries (testSentry sentryclient ${CPPNETLIB_LIBRARIES})

# About:

This is a very basic C++-11 client for Sentry (https://sentry.io/)

Wrote this in a day so it's not tested properly.


# Dependencies:

1. CPP-Netlib (http://cpp-netlib.org/)
2. JSON for Modern C++ (https://github.com/nlohmann/json)


# Compilation:

1. Please see dependencies above.
2. This is a header only library so no need to build anything. Just include
   "sentry.hpp" and link the dependencies with your executable / library.
3. To build the test:

    > git clone git@github.com:shikharkhattar/sentry-cpp.git  
    > cd sentry-cpp  
    > mkdir build  
    > cd build  
    > cmake ..  
    > make  


# Use it

See test/test.cpp (Change the Sentry DSN)


# Notes:

1. Sentry API documentation is [here] (https://docs.sentry.io/clientdev/overview/)
but only the minimum required functionality is provided as of now.
2. Lots of TODOs scattered around the code which should reflect the open issues.


# License

MIT License. Please see LICENSE

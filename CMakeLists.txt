# Overall project specification.  Specify the C++ and CMake 
# versions required.

cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()

# Define the project. 

project(ScalaFileRead VERSION 0.1.1)

find_package(Boost 1.70.0 CONFIG REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# This is our library, and we'll link against it.

add_subdirectory(src)

# These are our tests.  They are a bunch of small executables
# that load various scale files.
add_subdirectory(tests)


# Installation
install(FILES README.rst DESTINATION share/doc/libscala-file)
install(FILES LICENSE DESTINATION share/doc/libscala-file)

# Stuff that Code added 

include(CTest)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

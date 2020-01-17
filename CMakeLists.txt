cmake_minimum_required(VERSION 3.0.0)
project(HercControl VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(cpr)

add_executable(HercControl src/HercControl.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

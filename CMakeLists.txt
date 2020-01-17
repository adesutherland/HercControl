cmake_minimum_required(VERSION 3.0.0)
project(HercControl VERSION 0.1.0)

include(CTest)
enable_testing()

option(HTTP_ONLY "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)" OFF)
add_subdirectory(cpr)

add_executable(HercControl src/HercControl.cpp)
target_link_libraries(HercControl PUBLIC cpr)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

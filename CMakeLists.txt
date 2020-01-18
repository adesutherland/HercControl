cmake_minimum_required(VERSION 3.0.0)
project(HercControl VERSION 0.1.0)

#include(CTest)
#enable_testing()

if(WIN32)
    add_definitions("-DCURL_STATICLIB")
endif()
option(CURL_STATICLIB "Set to ON to build libcurl with static linking." ON)
option(BUILD_CPR_TESTS "Set to ON to build cpr tests." OFF)
option(CMAKE_USE_OPENSSL "Use OpenSSL code. Experimental" OFF)
option(HTTP_ONLY "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)" ON)
add_subdirectory(cpr)

add_executable(HercControl src/HercControl.cpp)
target_link_libraries(HercControl PUBLIC cpr)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

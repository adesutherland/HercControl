cmake_minimum_required(VERSION 3.0.0)
project(HercControl VERSION 0.1.7)

#include(CTest)
#enable_testing()

# Might as well share libraries on Ubuntu ...
# SET(CMAKE_EXE_LINKER_FLAGS "-static")
option(CURL_STATICLIB "Set to ON to build libcurl with static linking." ON)
add_definitions("-DCURL_STATICLIB")
if(WIN32)
   # On windows we will link static to help portability
   set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()

option(HTTP_ONLY "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)" ON)
option(CMAKE_USE_LIBSSH2 "Use libSSH2" OFF)
set(CURL_CA_PATH "none" CACHE STRING
    "Location of default CA path. Set 'none' to disable or 'auto' for auto-detection. Defaults to 'auto'.")
option(CMAKE_USE_OPENSSL "Use OpenSSL code. Experimental" OFF)

option(BUILD_CPR_TESTS "Set to ON to build cpr tests." OFF)

configure_file(src/config.h.in include/config.h)

add_subdirectory(cpr)
 
add_executable(HercControl src/HercControl.cpp)
target_link_libraries(HercControl PUBLIC cpr)
target_include_directories(HercControl PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories(HercControl PUBLIC rang/include)
target_include_directories(HercControl PUBLIC CLI11/include)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)

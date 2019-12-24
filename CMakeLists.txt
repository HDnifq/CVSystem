cmake_minimum_required(VERSION 2.8.11)

#增加这个wrapper工程让vscode的cmake插件可以工作
project(cmake_wrapper)

set(CMAKE_BINARY_DIR build)
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")

#从build目录里去得到conanbuildinfo.cmake
set(CONAN_BUILD_INFO_PATH ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(CheckIncludeFile)

add_subdirectory("src")

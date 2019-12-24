# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
#
cmake_minimum_required(VERSION 3.0)

# 设置一个工具链,如果进入设置了那么通常是能够正确工作,如果有被其他设置所覆盖那么可能无法工作
# if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#   set(CMAKE_TOOLCHAIN_FILE "/home/dx/vcpkg/scripts/buildsystems/vcpkg.cmake"
#       CACHE STRING "")
#   message(STATUS "not defined CMAKE_TOOLCHAIN_FILE !")
# endif()

# message(STATUS "vcpkg.cmake = " ${CMAKE_TOOLCHAIN_FILE})

# if(NOT DEFINED DVCPKG_TARGET_TRIPLET)
#   set(DVCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
# endif()

# 在上面设置工具链之后下面要立马project命令
project(CVSystem VERSION 2.0.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 14)

message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")

# 在有了project之后需要第一时间搞个conan设置吧
if(NOT DEFINED CONAN_BUILD_INFO_PATH)
    # 如果没有设置过
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()
# conan_set_vs_runtime()

# 设置是否编译Test
option(CVSYSTEM_BUILD_TESTS "CVSYSTEM_BUILD_TESTS" ON)
message(STATUS "CVSYSTEM_BUILD_TESTS = ${CVSYSTEM_BUILD_TESTS}" )
# 设置是否编译成共享库
option(CVSYSTEM_BUILD_SHARED "CVSYSTEM_BUILD_SHARED" ON)
message(STATUS "CVSYSTEM_BUILD_SHARED = ${CVSYSTEM_BUILD_SHARED}" )

add_definitions(-D DLOG_UTF8)
add_definitions(-D _NO_ASYNCRTIMP)

# 设置生成一个VS工程里的文件夹
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 修改一些默认值 Use this snippet *after* PROJECT(xxx):
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install CACHE PATH "Install directory" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

 
# 设置工程外部库的文件夹 $ENV{HOME}
if(WIN32)
set(PROJECT_LIB_DIR "C:/dxlib/lib" CACHE STRING "dx's lib")
else()
set(PROJECT_LIB_DIR $ENV{HOME}/dxlib/lib CACHE STRING "dx's lib")
endif()
message(STATUS "PROJECT_LIB_DIR = " ${PROJECT_LIB_DIR})

# 现在使用了conan库,这里并不需要了
# if(WIN32)
# # 设置编译选项为MT https://stackoverflow.com/questions/14172856/compile-with-mt-instead-of-md-using-cmake
#     set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
#     set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
# endif()

#设置文件输出的目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(CVSystem)

if(CVSYSTEM_BUILD_TESTS)
# 使能测试
enable_testing()
include(CTest)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/CVSTest)
endif()
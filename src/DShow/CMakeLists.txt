cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SUB_NAME "baseclasses")

# 定义UNICODE会是TCHAR为wchar
# add_definitions(-D UNICODE)
if(WIN32)
    add_definitions(-D _WINDOWS)
    add_definitions(-D _USERDLL)
    # 这个设置只支持windows，所以不要使用了
    # add_definitions(-D SPDLOG_WCHAR_TO_UTF8_SUPPORT)
endif()

# 先自动添加算了
FILE(GLOB_RECURSE SOURCE_LIST "*.cpp" "*.h" "*.hpp" "*.c")

# 这个函数在VS工程里生成一个文件路径
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        #这个表示在VS里划分一个组
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${SOURCE_LIST})
message(STATUS "SOURCE_LIST = ${SOURCE_LIST}")

#静态库
add_library(${SUB_NAME}
            STATIC
            ${SOURCE_LIST})

target_include_directories(${SUB_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/baseclasses)
target_link_libraries(${SUB_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT})


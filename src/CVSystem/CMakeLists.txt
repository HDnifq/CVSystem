cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SUB_NAME "CVSystem")

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

#这里的命名需要有.out,否则编译arm的时候不能运行...
# Library definition
if(CVSYSTEM_BUILD_SHARED)
    add_library(${SUB_NAME}
                SHARED
                ${SOURCE_LIST})
else()
    add_library(${SUB_NAME}
                STATIC
                ${SOURCE_LIST})
endif()
 
# 设置dlog库的库目录,x64版本
# if(WIN32)
#     if(NOT DEFINED dlog_LIB_DIR)
#         set(dlog_LIB_DIR ${PROJECT_LIB_DIR}/dlog/x64)
#     endif()
#     set(dlog_LIB ${dlog_LIB_DIR}/dlog.lib)
#     #拷贝dlog.dll到安装目录
#     # add_custom_command(TARGET app POST_BUILD        # Adds a post-build event to MyTest
#     #     COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#     #         "${PROJECT_LIB_DIR}/dlog/x64/dlog.dll"      # <--this is in-file
#     #         ${CMAKE_INSTALL_PREFIX}/dlog.dll)                 # <--this is out-file path
    
#     # 这里的复制没有太好的方法,因为vs会自己多出Release文件夹来,只能把文件夹名字写死了
#     # https://stackoverflow.com/questions/10671916/how-to-copy-dll-files-into-the-same-folder-as-the-executable-using-cmake?rq=1
#     # https://stackoverflow.com/questions/39282430/how-to-get-current-configuration-release-debug-in-cmake-for-visual-studio
#     add_custom_command(TARGET ${SUB_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dlog_LIB_DIR}/dlog.dll" ${EXECUTABLE_OUTPUT_PATH}/Release/dlog.dll)
#     add_custom_command(TARGET ${SUB_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dlog_LIB_DIR}/dlog.dll" ${EXECUTABLE_OUTPUT_PATH}/RelWithDebInfo/dlog.dll)
# else()
#     if(NOT DEFINED dlog_LIB_DIR)
#         set(dlog_LIB_DIR ${PROJECT_LIB_DIR}/dlog)
#     endif()
#     set(dlog_LIB ${dlog_LIB_DIR}/libdlog.a)  
#     # 其他平台目前是静态编译的，就没有拷贝dll
# endif()
# if(CROSS_COMPILE)
#     set(dlog_LIB ${dlog_LIB_DIR}_arm64/libdlog.a)
# endif()
# message(STATUS "dlog_LIB = " ${dlog_LIB})

# # 设置boost库
# find_package(Boost 1.70.0 REQUIRED COMPONENTS filesystem date_time)
# message(STATUS "Boost_INCLUDE_DIRS = " ${Boost_INCLUDE_DIRS})
# message(STATUS "Boost_LIBRARIES = " ${Boost_LIBRARIES})

# # 设置opencv库
# find_package(OpenCV REQUIRED)
# message(STATUS "OpenCV_INCLUDE_DIRS = " ${OpenCV_INCLUDE_DIRS})
# message(STATUS "OpenCV_LIBS = " ${OpenCV_LIBS})
# if(WIN32)
#     set(OpenCV_LIBS_DIR ${OpenCV_DIR}/x64/vc15/bin)
#     add_custom_command(TARGET ${SUB_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory "${OpenCV_LIBS_DIR}" ${EXECUTABLE_OUTPUT_PATH}/Release) #拷贝整个文件夹算了
#     add_custom_command(TARGET ${SUB_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory "${OpenCV_LIBS_DIR}" ${EXECUTABLE_OUTPUT_PATH}/RelWithDebInfo)
# endif()

find_package (Threads)

message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "CONAN_LIBS = ${CONAN_LIBS}")

target_include_directories(${SUB_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
if(WIN32)
    target_include_directories(${SUB_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../DShow)
endif()

target_link_libraries(${SUB_NAME} PRIVATE ${CONAN_LIBS} ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
    #链接show的库
    target_link_libraries(${SUB_NAME} PRIVATE baseclasses)
endif()

# 全都安装到根目录
install(TARGETS ${SUB_NAME}
       RUNTIME DESTINATION "./"
       LIBRARY DESTINATION "./"
       ARCHIVE DESTINATION "./"
)
# 安装头文件
# install(FILES "dlog.h" DESTINATION "./")
# install(FILES "../UnitTest/DLog.cs" DESTINATION "./")

# Configuration
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

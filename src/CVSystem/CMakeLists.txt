cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SUB_NAME "CVSystem")

#这里的命名需要有.out,否则编译arm的时候不能运行...
add_library(${SUB_NAME}
            STATIC
            Common.h
            cvsdef.h
            CVSystem.h
            Json.h
            stdafx.h
            targetver.h
            Common/BaseThread.h
            Common/blockingconcurrentqueue.h
            Common/concurrentqueue.h
            Common/Event.h
            Common/StringHelper.h
            DTO/DTOType.h
            DTO/JsonHelper.h
            DTO/Serialize.h
            DTO/sht.h
            eventbus/AsyncEventBus.h
            eventbus/EventBus.h
            eventbus/EventCollector.h
            eventbus/TokenHolder.h
            eventbus/internal/AsyncCallbackVector.h
            eventbus/internal/CallbackVector.h
            eventbus/internal/common.h
            eventbus/internal/TransactionCallbackVector.h
            Proc/Camera.h
            Proc/CameraGrab.h
            Proc/CameraImage.h
            Proc/CameraManger.h
            Proc/CameraThread.h
            Proc/DevicesHelper.h
            Proc/Draw.h
            Proc/FrameProc.h
            Proc/MultiCamera.h
            Proc/MultiCameraMT.h
            ALGO.hpp
            Common/CoolTime.hpp
            Common/FPSCalc.hpp
            Proc/CamImageProc.hpp
            Proc/SimpleModule.hpp
            CVSystem.cpp
            stdafx.cpp
            Common/BaseThread.cpp
            Common/Event.cpp
            Common/StringHelper.cpp
            eventbus/AsyncEventBus.cpp
            Proc/Camera.cpp
            Proc/CameraGrab.cpp
            Proc/CameraManger.cpp
            Proc/CameraThread.cpp
            Proc/DevicesHelper.cpp
            Proc/Draw.cpp
            Proc/MultiCamera.cpp
            Proc/MultiCameraMT.cpp
            )

# 设置dlog库的库目录,x64版本
if(WIN32)
    if(NOT DEFINED dlog_LIB_DIR)
        set(dlog_LIB_DIR ${PROJECT_LIB_DIR}/dlog/x64)
    endif()
    set(dlog_LIB ${dlog_LIB_DIR}/dlog.lib)
    #拷贝dlog.dll到安装目录
    # add_custom_command(TARGET app POST_BUILD        # Adds a post-build event to MyTest
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
    #         "${PROJECT_LIB_DIR}/dlog/x64/dlog.dll"      # <--this is in-file
    #         ${CMAKE_INSTALL_PREFIX}/dlog.dll)                 # <--this is out-file path
    
    # 这里的复制没有太好的方法,因为vs会自己多出Release文件夹来,只能把文件夹名字写死了
    # https://stackoverflow.com/questions/10671916/how-to-copy-dll-files-into-the-same-folder-as-the-executable-using-cmake?rq=1
    # https://stackoverflow.com/questions/39282430/how-to-get-current-configuration-release-debug-in-cmake-for-visual-studio
    add_custom_command(TARGET ${SUB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dlog_LIB_DIR}/dlog.dll" ${EXECUTABLE_OUTPUT_PATH}/Release/dlog.dll)
    add_custom_command(TARGET ${SUB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dlog_LIB_DIR}/dlog.dll" ${EXECUTABLE_OUTPUT_PATH}/RelWithDebInfo/dlog.dll)
else()
    if(NOT DEFINED dlog_LIB_DIR)
        set(dlog_LIB_DIR ${PROJECT_LIB_DIR}/dlog)
    endif()
    set(dlog_LIB ${dlog_LIB_DIR}/libdlog.a)  
    # 其他平台目前是静态编译的，就没有拷贝dll
endif()
if(CROSS_COMPILE)
    set(dlog_LIB ${dlog_LIB_DIR}_arm64/libdlog.a)
endif()
message(STATUS "dlog_LIB = " ${dlog_LIB})

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

target_include_directories(${SUB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}
                                            ${PROJECT_LIB_DIR}
                                            ${eigen_INCLUDE_DIR}
                                            ${rapidjson_INCLUDE_DIR}
                                            )
target_link_libraries(${SUB_NAME} PRIVATE ${dlog_LIB} ${CONAN_LIBS} ${CMAKE_THREAD_LIBS_INIT})

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

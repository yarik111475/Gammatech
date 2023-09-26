cmake_minimum_required(VERSION 3.14)
set(PROJECT_NAME Gammatech)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#spdlog
set(SPDLOG_MASTER_PROJECT OFF)
set(SPDLOG_INSTALL OFF)
set(SPDLOG_BUILD_SHARED OFF)
set(SPDLOG_WCHAR_FILENAMES ON)

#rpath
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH ";../lib;../lib64;")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(WIN32)
    set(WIN32_LINKER_LIBS
        ws2_32
        user32
        netapi32
        iphlpapi
    )
endif()

if(UNIX AND NOT APPLE)
    set(LINUX_LINKER_LIBS
        -static-libgcc
        -static-libstdc++
         -pthread
    )
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Client)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Simulator)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Interface)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/3rdparty/spdlog-1.9.2)

cmake_minimum_required(VERSION 3.14)
set(PROJECT_NAME Client)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "*.h"
    "*.cpp"
)

find_package(Qt5 COMPONENTS Charts REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

add_executable(${PROJECT_NAME} WIN32
  ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME} 
    Qt5::Core
    Qt5::Charts
    Qt5::Widgets
    Qt5::Network
    Qt5::PrintSupport
    ${WIN32_LINKER_LIBS}
    ${LINUX_LINKER_LIBS}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

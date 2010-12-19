project(Koushin)

cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

FIND_LIBRARY(QT_QTSCRIPT_TOOLS_LIBRARY_RELEASE NAMES QtScriptTools QtScriptTools4 PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)

# message("${KDE4_DEFINITIONS}:" ${KDE4_DEFINITIONS})
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
remove_definitions(-DQT_NO_STL)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
    ${KDEGAMES_INCLUDE_DIRS}
)

add_subdirectory(src)
add_subdirectory(data)

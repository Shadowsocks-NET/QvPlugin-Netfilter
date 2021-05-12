project(QvPlugin-WinNetFilter)
cmake_policy(SET CMP0003 NEW)
cmake_policy(SET CMP0012 NEW)
cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui Network REQUIRED)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(${CMAKE_SOURCE_DIR}/interface/QvPluginInterface.cmake)

add_definitions(-DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0A000000 -DWIN32_LEAN_AND_MEAN)

add_library(${PROJECT_NAME} MODULE
    ${CMAKE_SOURCE_DIR}/NetfilterPlugin.hpp
    ${CMAKE_SOURCE_DIR}/NetfilterPlugin.cpp
    ${CMAKE_SOURCE_DIR}/core/Settings.hpp
    ${CMAKE_SOURCE_DIR}/core/EventHandler.cpp
    ${CMAKE_SOURCE_DIR}/core/EventHandler.hpp
    ${CMAKE_SOURCE_DIR}/ui/Interface.hpp
    ${CMAKE_SOURCE_DIR}/ui/PluginMainWindowWidget.cpp
    ${CMAKE_SOURCE_DIR}/ui/PluginMainWindowWidget.hpp
    ${CMAKE_SOURCE_DIR}/ui/PluginMainWindowWidget.ui
    ${CMAKE_SOURCE_DIR}/ui/PluginSettingsWidget.cpp
    ${CMAKE_SOURCE_DIR}/ui/PluginSettingsWidget.hpp
    ${CMAKE_SOURCE_DIR}/ui/PluginSettingsWidget.ui
    ${CMAKE_SOURCE_DIR}/resx.qrc
    #
    ${CMAKE_SOURCE_DIR}/netfilter/UDPContext.hpp
    ${CMAKE_SOURCE_DIR}/netfilter/iocp.h
    ${CMAKE_SOURCE_DIR}/netfilter/linkedlist.h
    ${CMAKE_SOURCE_DIR}/netfilter/SOCKS.h
    ${CMAKE_SOURCE_DIR}/netfilter/SocksRedirector.cpp
    ${CMAKE_SOURCE_DIR}/netfilter/SocksRedirector.hpp
    ${CMAKE_SOURCE_DIR}/netfilter/NFEventHandler.cpp
    ${CMAKE_SOURCE_DIR}/netfilter/NFEventHandler.hpp
    ${CMAKE_SOURCE_DIR}/netfilter/sync.h
    ${CMAKE_SOURCE_DIR}/netfilter/UDPProxy.h
    ${CMAKE_SOURCE_DIR}/netfilter/base.h
    ${CMAKE_SOURCE_DIR}/netfilter/UDPProxy.cpp
    ${CMAKE_SOURCE_DIR}/netfilter/UTF-8.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/netfilter/include)

qv2ray_configure_plugin(${PROJECT_NAME} Widgets)
target_link_libraries(${PROJECT_NAME} ws2_32 wininet ${CMAKE_SOURCE_DIR}/netfilter/lib/${CMAKE_BUILD_TYPE}/nfapi.lib)
target_link_libraries(${PROJECT_NAME} Qt::Network)

cmake_minimum_required(VERSION 3.1.0)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0003 NEW)
cmake_policy(SET CMP0012 NEW)
cmake_policy(SET CMP0100 NEW)
cmake_policy(SET CMP0071 NEW)

project(NetFilterPlugin LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui Network REQUIRED)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(interface)

add_definitions(-DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0A000000 -DWIN32_LEAN_AND_MEAN)

add_library(NetFilterPlugin SHARED
    ${CMAKE_CURRENT_LIST_DIR}/NetfilterPlugin.hpp
    ${CMAKE_CURRENT_LIST_DIR}/NetfilterPlugin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/Settings.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/EventHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/EventHandler.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/Interface.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginMainWindowWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginMainWindowWidget.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginMainWindowWidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginSettingsWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginSettingsWidget.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/PluginSettingsWidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/resx.qrc
    #
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/UDPContext.hpp
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/iocp.h
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/linkedlist.h
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/SOCKS.h
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/SocksRedirector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/SocksRedirector.hpp
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/NFEventHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/NFEventHandler.hpp
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/sync.h
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/base.h
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/UDPProxy.h
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/UDPProxy.cpp
    ${CMAKE_CURRENT_LIST_DIR}/netfilter/UTF-8.h)

target_include_directories(NetFilterPlugin PRIVATE ${CMAKE_CURRENT_LIST_DIR}/netfilter/include)

qv2ray_configure_plugin(NetFilterPlugin Widgets
    EXTRA_DEPENDENCY_DIRS_WINDOWS "${CMAKE_CURRENT_LIST_DIR}/netfilter/bin/release")

target_link_libraries(NetFilterPlugin PRIVATE ws2_32 wininet ${CMAKE_CURRENT_LIST_DIR}/netfilter/lib/${CMAKE_BUILD_TYPE}/nfapi.lib)
target_link_libraries(NetFilterPlugin PRIVATE Qt::Network)

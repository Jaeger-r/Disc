if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.21)
    project(DiskClientStandalone LANGUAGES C CXX)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    if(APPLE)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS deployment target" FORCE)
        if(NOT CMAKE_OSX_SYSROOT)
            if(EXISTS "/Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk")
                set(JAEGER_MACOS_SDK "/Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk")
            else()
                execute_process(
                    COMMAND xcrun --show-sdk-path
                    OUTPUT_VARIABLE JAEGER_MACOS_SDK
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                )
            endif()
            if(JAEGER_MACOS_SDK)
                set(CMAKE_OSX_SYSROOT "${JAEGER_MACOS_SDK}" CACHE STRING "macOS SDK path" FORCE)
            endif()
        endif()
    endif()

    set(JAEGER_QT_ROOT "$ENV{JAEGER_QT_ROOT}")
    if(NOT JAEGER_QT_ROOT AND APPLE)
        foreach(JAEGER_QT_CANDIDATE
                "$ENV{HOME}/Qt/6.11.1/macos"
                "/Volumes/Jaeger/Qt/6.9.1/macos")
            if(EXISTS "${JAEGER_QT_CANDIDATE}")
                set(JAEGER_QT_ROOT "${JAEGER_QT_CANDIDATE}")
                break()
            endif()
        endforeach()
    endif()
    if(JAEGER_QT_ROOT)
        list(PREPEND CMAKE_PREFIX_PATH "${JAEGER_QT_ROOT}")
    endif()

    find_package(Qt6 6.9.1 REQUIRED COMPONENTS Core Gui Widgets Network)
endif()

set(DISKCLIENT_SOURCES
    chat.cpp
    imagecropdialog.cpp
    login.cpp
    main.cpp
    md5.cpp
    transferpanel.cpp
    widget.cpp
    client/tcpnet.cpp
    kernel/tcpkernel.cpp
)

set(DISKCLIENT_HEADERS
    Packdef.h
    chat.h
    imagecropdialog.h
    login.h
    md5.h
    transferpanel.h
    version.h
    widget.h
    client/INet.h
    client/tcpnet.h
    kernel/IKernel.h
    kernel/tcpkernel.h
)

set(DISKCLIENT_UIS
    chat.ui
    login.ui
    widget.ui
)

set(DISKCLIENT_RESOURCES
    resources.qrc
)

set(DISKCLIENT_APP_ICON "")
if(APPLE)
    set(DISKCLIENT_APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/DiskClient.icns")
endif()

qt_add_executable(DiskClientApp
    ${DISKCLIENT_SOURCES}
    ${DISKCLIENT_HEADERS}
    ${DISKCLIENT_UIS}
    ${DISKCLIENT_RESOURCES}
    ${DISKCLIENT_APP_ICON}
)

target_include_directories(DiskClientApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel
)

target_link_libraries(DiskClientApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

set_target_properties(DiskClientApp PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "DiskClient"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.jaeger.diskclient"
    MACOSX_BUNDLE_ICON_FILE "DiskClient.icns"
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME DiskClient
)

if(APPLE)
    set_source_files_properties("${DISKCLIENT_APP_ICON}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

add_custom_command(TARGET DiskClientApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/diskclient.ini"
        "$<TARGET_FILE_DIR:DiskClientApp>/diskclient.ini"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:DiskClientApp>/tls"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/tls/ca.crt"
        "$<TARGET_FILE_DIR:DiskClientApp>/tls/ca.crt"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/file.png"
        "$<TARGET_FILE_DIR:DiskClientApp>/file.png"
)

qt_add_executable(DiskClientSmokeTest
    tests/smoke_test.cpp
    Packdef.h
)

target_include_directories(DiskClientSmokeTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(DiskClientSmokeTest PRIVATE
    Qt6::Core
    Qt6::Network
)

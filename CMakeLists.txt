cmake_minimum_required(VERSION 3.16)
project(alexandria VERSION 0.1.0 LANGUAGES C CXX)

option(BUILD_CLIENT "Build the QML desktop client" ON)

option(BUILD_CLIENT "Build the QML desktop client" ON)
option(ALEXANDRIA_SERVER_TRAY "Build the server with a native system tray icon (desktop only; disable for headless/Docker deployments)" ON)

if(BUILD_CLIENT)
    find_package(Qt6 REQUIRED COMPONENTS Core Sql Quick QuickControls2 QuickDialogs2 Concurrent Test)
else()
    find_package(Qt6 REQUIRED COMPONENTS Core Sql Test)
endif()

if(ALEXANDRIA_SERVER_TRAY)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
endif()

qt_standard_project_setup(REQUIRES 6.5)

option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_subdirectory(proto)
add_subdirectory(server)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()
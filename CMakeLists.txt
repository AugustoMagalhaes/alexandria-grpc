cmake_minimum_required(VERSION 3.16)
project(alexandria VERSION 0.1.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Quick QuickControls2 Test)
qt_standard_project_setup(REQUIRES 6.10)

option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
    enable_testing()
endif()

add_subdirectory(server)
add_subdirectory(client)

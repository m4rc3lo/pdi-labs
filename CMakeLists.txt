cmake_minimum_required(VERSION 3.24)

project(
    pdi_labs
    VERSION 0.1.0
    DESCRIPTION "Didactic Image Processing laboratory implementations"
    LANGUAGES CXX
)

option(PDI_BUILD_TESTS "Build the automated test suite" ON)
option(PDI_BUILD_DOCS "Build the Doxygen documentation" OFF)
option(PDI_BUILD_EXAMPLES "Build laboratory and example applications" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CompilerWarnings)

find_package(OpenCV REQUIRED COMPONENTS core)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/pdi/version.hpp"
    @ONLY
)

add_library(pdi_core
    src/core/project_info.cpp
)

add_library(pdi::core ALIAS pdi_core)

target_include_directories(pdi_core
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>"
        "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(pdi_core
    PUBLIC
        opencv_core
)

pdi_enable_project_warnings(pdi_core)

if(PDI_BUILD_EXAMPLES)
    add_executable(pdi_info
        apps/pdi_info/main.cpp
    )

    target_link_libraries(pdi_info
        PRIVATE
            pdi::core
    )

    pdi_enable_project_warnings(pdi_info)
endif()

if(PDI_BUILD_TESTS)
    include(CTest)
    enable_testing()
endif()

if(PDI_BUILD_DOCS)
    message(
        STATUS
        "PDI_BUILD_DOCS is enabled; Doxygen integration will be added in a later increment."
    )
endif()

message(STATUS "pdi-labs version: ${PROJECT_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "Build tests: ${PDI_BUILD_TESTS}")
message(STATUS "Build documentation: ${PDI_BUILD_DOCS}")
message(STATUS "Build examples: ${PDI_BUILD_EXAMPLES}")

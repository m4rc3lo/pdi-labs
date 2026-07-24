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
option(
    PDI_FETCH_TEST_DEPENDENCIES
    "Allow CMake to download missing test dependencies"
    ON
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CompilerWarnings)

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs highgui)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/pdi/version.hpp"
    @ONLY
)

add_library(pdi_core
    src/core/image_validator.cpp
    src/core/project_info.cpp
    src/core/saturation.cpp
    src/value/channel_separator.cpp
    src/value/grayscale_converter.cpp
    src/value/grayscale_quantizer.cpp
    src/value/image_copy.cpp
    src/value/image_inspector.cpp
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

add_library(pdi_io
    src/io/image_display.cpp
    src/io/image_file_storage.cpp
)

add_library(pdi::io ALIAS pdi_io)

target_include_directories(pdi_io
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(pdi_io
    PUBLIC
        opencv_core
    PRIVATE
        opencv_highgui
        opencv_imgcodecs
)

pdi_enable_project_warnings(pdi_io)

if(PDI_BUILD_EXAMPLES)
    add_executable(pdi_info
        apps/pdi_info/main.cpp
    )

    target_link_libraries(pdi_info
        PRIVATE
            pdi::core
    )

    pdi_enable_project_warnings(pdi_info)

    add_executable(lab_m1_1_channels
        apps/lab_m1_1_channels/main.cpp
    )

    target_link_libraries(lab_m1_1_channels
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_1_channels)

    add_executable(lab_m1_1_grayscale
        apps/lab_m1_1_grayscale/main.cpp
    )

    target_link_libraries(lab_m1_1_grayscale
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_1_grayscale)

    add_executable(lab_m1_1_quantization
        apps/lab_m1_1_quantization/main.cpp
    )

    target_link_libraries(lab_m1_1_quantization
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_1_quantization)
endif()

if(PDI_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

if(PDI_BUILD_DOCS)
    find_package(Doxygen REQUIRED)

    find_program(PDI_DOT_EXECUTABLE dot)

    if(PDI_DOT_EXECUTABLE)
        set(PDI_DOXYGEN_HAVE_DOT YES)
        message(STATUS "Graphviz dot found: ${PDI_DOT_EXECUTABLE}")
    else()
        set(PDI_DOXYGEN_HAVE_DOT NO)
        message(STATUS "Graphviz dot was not found; Doxygen diagrams will be disabled.")
    endif()

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
        "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        @ONLY
    )

    add_custom_target(docs
        COMMAND
            "${DOXYGEN_EXECUTABLE}"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating pdi-labs HTML documentation with Doxygen"
        VERBATIM
    )
endif()

message(STATUS "pdi-labs version: ${PROJECT_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "Build tests: ${PDI_BUILD_TESTS}")
message(STATUS "Build documentation: ${PDI_BUILD_DOCS}")
message(STATUS "Build examples: ${PDI_BUILD_EXAMPLES}")
message(STATUS "Fetch test dependencies: ${PDI_FETCH_TEST_DEPENDENCIES}")

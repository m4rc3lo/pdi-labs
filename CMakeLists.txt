cmake_minimum_required(VERSION 3.24)

project(
    pdi_labs
    VERSION 0.5.0
    DESCRIPTION "Didactic Image Processing laboratory implementations"
    LANGUAGES CXX
)

option(PDI_BUILD_TESTS "Build the automated test suite" ON)
option(PDI_BUILD_DOCS "Build the Doxygen documentation" OFF)
option(PDI_BUILD_EXAMPLES "Build laboratory and example applications" ON)
option(
    PDI_BUILD_INTERACTIVE_UI
    "Build optional HighGUI interactive components"
    OFF
)
option(
    PDI_BUILD_INTERACTIVE_TESTS
    "Build manual interactive HighGUI diagnostics"
    OFF
)
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

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs highgui imgproc)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/pdi/version.hpp"
    @ONLY
)

add_library(pdi_windowing
    src/windowing/window_lifecycle.cpp
)

add_library(pdi::windowing ALIAS pdi_windowing)

target_include_directories(pdi_windowing
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)

pdi_enable_project_warnings(pdi_windowing)

add_library(pdi_core
    src/core/image_validator.cpp
    src/core/project_info.cpp
    src/core/saturation.cpp
    src/value/channel_separator.cpp
    src/value/grayscale_converter.cpp
    src/value/grayscale_quantizer.cpp
    src/value/image_copy.cpp
    src/value/image_inspector.cpp
    src/labs/m1_1_pipeline.cpp
    src/labs/m1_2_study_case.cpp
    src/labs/m1_3_pipeline.cpp
    src/labs/m2_1_pipeline.cpp
    src/spatial/spatial_convolution.cpp
    src/spatial/spatial_kernels.cpp
    src/spatial/smoothing_filters.cpp
    src/spatial/laplacian_enhancement.cpp
    src/spatial/sobel_operator.cpp
    src/segmentation/manual_threshold.cpp
    src/segmentation/mask_operations.cpp
    src/segmentation/library_segmentation_pipeline.cpp
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
        opencv_imgproc
)

pdi_enable_project_warnings(pdi_core)

add_library(pdi_io
    src/io/image_display.cpp
    src/io/image_file_storage.cpp
    src/io/processing_data_storage.cpp
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
        pdi::windowing
        opencv_highgui
        opencv_imgcodecs
)

pdi_enable_project_warnings(pdi_io)

if(PDI_BUILD_INTERACTIVE_UI)
    add_library(pdi_ui
        src/ui/interactive_state.cpp
        src/ui/interactive_window.cpp
        src/ui/keyboard_event.cpp
        src/ui/keyboard_event_dispatcher.cpp
        src/ui/mouse_event.cpp
        src/ui/qt_controls.cpp
        src/ui/ui_capabilities.cpp
    )

    add_library(pdi::ui ALIAS pdi_ui)

    target_include_directories(pdi_ui
        PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
    )

    target_link_libraries(pdi_ui
        PUBLIC
            pdi::windowing
            opencv_core
        PRIVATE
            opencv_highgui
    )

    pdi_enable_project_warnings(pdi_ui)
endif()

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

    add_executable(lab_m1_1
        apps/lab_m1_1/main.cpp
    )

    target_link_libraries(lab_m1_1
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_1)

    add_executable(lab_m1_2
        apps/lab_m1_2/main.cpp
    )

    target_link_libraries(lab_m1_2
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_2)

    add_executable(lab_m1_3_smoothing
        apps/lab_m1_3_smoothing/main.cpp
    )

    target_link_libraries(lab_m1_3_smoothing
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_3_smoothing)

    add_executable(lab_m1_3_laplacian
        apps/lab_m1_3_laplacian/main.cpp
    )

    target_link_libraries(lab_m1_3_laplacian
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_3_laplacian)

    add_executable(lab_m1_3_sobel
        apps/lab_m1_3_sobel/main.cpp
    )

    target_link_libraries(lab_m1_3_sobel
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_3_sobel)

    add_executable(lab_m1_3
        apps/lab_m1_3/main.cpp
    )

    target_link_libraries(lab_m1_3
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m1_3)

    add_executable(lab_m2_1
        apps/lab_m2_1/main.cpp
    )

    target_link_libraries(lab_m2_1
        PRIVATE
            pdi::core
            pdi::io
    )

    if(PDI_BUILD_INTERACTIVE_UI)
        target_link_libraries(lab_m2_1 PRIVATE pdi::ui)
        target_compile_definitions(
            lab_m2_1
            PRIVATE PDI_HAS_INTERACTIVE_UI=1
        )
    endif()

    pdi_enable_project_warnings(lab_m2_1)

    add_executable(lab_m2_1_library
        apps/lab_m2_1_library/main.cpp
    )

    target_link_libraries(lab_m2_1_library
        PRIVATE
            pdi::core
            pdi::io
    )

    pdi_enable_project_warnings(lab_m2_1_library)

    if(PDI_BUILD_INTERACTIVE_UI)
        add_executable(lab_m2_1_interactive
            apps/lab_m2_1_interactive/main.cpp
        )

        target_link_libraries(lab_m2_1_interactive
            PRIVATE
                pdi::core
                pdi::io
                pdi::ui
        )

        pdi_enable_project_warnings(lab_m2_1_interactive)
    endif()
endif()

if(PDI_BUILD_INTERACTIVE_UI AND PDI_BUILD_INTERACTIVE_TESTS)
    add_executable(pdi_ui_test_trackbar
        apps/ui_diagnostics/trackbar/main.cpp
    )
    target_link_libraries(pdi_ui_test_trackbar PRIVATE pdi::ui)
    pdi_enable_project_warnings(pdi_ui_test_trackbar)

    add_executable(pdi_ui_test_mouse
        apps/ui_diagnostics/mouse/main.cpp
    )
    target_link_libraries(pdi_ui_test_mouse PRIVATE pdi::ui)
    pdi_enable_project_warnings(pdi_ui_test_mouse)

    add_executable(pdi_ui_test_keyboard
        apps/ui_diagnostics/keyboard/main.cpp
    )
    target_link_libraries(pdi_ui_test_keyboard PRIVATE pdi::ui)
    pdi_enable_project_warnings(pdi_ui_test_keyboard)

    add_executable(pdi_ui_test_qt_controls
        apps/ui_diagnostics/qt_controls/main.cpp
    )
    target_link_libraries(pdi_ui_test_qt_controls PRIVATE pdi::ui)
    pdi_enable_project_warnings(pdi_ui_test_qt_controls)

    add_executable(pdi_ui_test_window_close
        apps/ui_diagnostics/window_close/main.cpp
    )
    target_link_libraries(pdi_ui_test_window_close PRIVATE pdi::ui)
    pdi_enable_project_warnings(pdi_ui_test_window_close)
elseif(PDI_BUILD_INTERACTIVE_TESTS)
    message(
        WARNING
        "PDI_BUILD_INTERACTIVE_TESTS is ignored because "
        "PDI_BUILD_INTERACTIVE_UI is OFF."
    )
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
message(STATUS "Build interactive UI: ${PDI_BUILD_INTERACTIVE_UI}")
message(STATUS "Build interactive diagnostics: ${PDI_BUILD_INTERACTIVE_TESTS}")
message(STATUS "Fetch test dependencies: ${PDI_FETCH_TEST_DEPENDENCIES}")

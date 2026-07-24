find_package(Catch2 3 CONFIG QUIET)

if(NOT Catch2_FOUND)
    if(NOT PDI_FETCH_TEST_DEPENDENCIES)
        message(
            FATAL_ERROR
            "Catch2 3 was not found and PDI_FETCH_TEST_DEPENDENCIES is OFF. "
            "Install Catch2 3 in the active toolchain or reconfigure with "
            "-DPDI_FETCH_TEST_DEPENDENCIES=ON."
        )
    endif()

    include(FetchContent)

    set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.15.0
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(Catch2)
endif()

if(DEFINED catch2_SOURCE_DIR)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
endif()

include(Catch)

add_library(pdi_test_helpers INTERFACE)

add_library(pdi::test_helpers ALIAS pdi_test_helpers)

target_include_directories(pdi_test_helpers
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(pdi_test_helpers
    INTERFACE
        Catch2::Catch2
        opencv_core
)

add_executable(pdi_unit_tests
    unit/channel_separator_test.cpp
    unit/grayscale_converter_test.cpp
    unit/image_copy_test.cpp
    unit/image_file_storage_test.cpp
    unit/image_inspector_test.cpp
    unit/image_validator_test.cpp
    unit/mat_comparison_test.cpp
    unit/project_info_test.cpp
    unit/saturation_test.cpp
)

target_link_libraries(pdi_unit_tests
    PRIVATE
        pdi::core
        pdi::io
        pdi::test_helpers
        Catch2::Catch2WithMain
)

pdi_enable_project_warnings(pdi_unit_tests)

catch_discover_tests(
    pdi_unit_tests
    TEST_PREFIX "unit."
)

add_executable(pdi_integration_tests
    integration/project_metadata_test.cpp
)

target_link_libraries(pdi_integration_tests
    PRIVATE
        pdi::core
        Catch2::Catch2WithMain
)

pdi_enable_project_warnings(pdi_integration_tests)

catch_discover_tests(
    pdi_integration_tests
    TEST_PREFIX "integration."
)

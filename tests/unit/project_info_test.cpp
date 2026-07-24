/**
 * @file project_info_test.cpp
 * @brief Tests access to configured project metadata.
 */

#include "pdi/core/project_info.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

TEST_CASE("Project metadata is available", "[unit][project-info]") {
    REQUIRE(pdi::core::ProjectInfo::project_name() == std::string_view{"pdi_labs"});
    REQUIRE_FALSE(pdi::core::ProjectInfo::project_version().empty());
    REQUIRE_FALSE(pdi::core::ProjectInfo::opencv_version().empty());
}

#include "pdi/core/project_info.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Core library exposes runtime dependency metadata", "[integration][metadata]") {
    REQUIRE_FALSE(pdi::core::ProjectInfo::project_name().empty());
    REQUIRE_FALSE(pdi::core::ProjectInfo::project_version().empty());
    REQUIRE_FALSE(pdi::core::ProjectInfo::opencv_version().empty());
}

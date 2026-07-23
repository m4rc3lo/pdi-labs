/**
 * @file saturation_test.cpp
 * @brief Tests unsigned 8-bit saturation and rounding.
 */

#include "pdi/core/saturation.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>

TEST_CASE("Saturation clamps values to the byte range", "[unit][core][saturation]") {
    REQUIRE(pdi::core::saturate_to_byte(-12.0) == std::uint8_t{0});
    REQUIRE(pdi::core::saturate_to_byte(0.0) == std::uint8_t{0});
    REQUIRE(pdi::core::saturate_to_byte(255.0) == std::uint8_t{255});
    REQUIRE(pdi::core::saturate_to_byte(300.0) == std::uint8_t{255});
}

TEST_CASE("Saturation rounds intermediate values", "[unit][core][saturation]") {
    REQUIRE(pdi::core::saturate_to_byte(42.4) == std::uint8_t{42});
    REQUIRE(pdi::core::saturate_to_byte(42.5) == std::uint8_t{43});
    REQUIRE(pdi::core::saturate_to_byte(254.6) == std::uint8_t{255});
}

TEST_CASE("Saturation handles non-finite values", "[unit][core][saturation]") {
    REQUIRE(
        pdi::core::saturate_to_byte(std::numeric_limits<double>::quiet_NaN())
        == std::uint8_t{0}
    );
    REQUIRE(
        pdi::core::saturate_to_byte(-std::numeric_limits<double>::infinity())
        == std::uint8_t{0}
    );
    REQUIRE(
        pdi::core::saturate_to_byte(std::numeric_limits<double>::infinity())
        == std::uint8_t{255}
    );
}

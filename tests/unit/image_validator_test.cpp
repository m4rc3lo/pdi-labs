/**
 * @file image_validator_test.cpp
 * @brief Tests the common cv::Mat validation rules.
 */

#include "pdi/core/image_validator.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <stdexcept>
#include <string>

TEST_CASE("Non-empty images pass the empty-image validation", "[unit][core][validation]") {
    const cv::Mat image(2, 3, CV_8UC1, cv::Scalar{0});

    REQUIRE_NOTHROW(pdi::core::ImageValidator::require_not_empty(image));
}

TEST_CASE("Empty images produce a clear validation error", "[unit][core][validation]") {
    const cv::Mat image;

    REQUIRE_THROWS_WITH(
        pdi::core::ImageValidator::require_not_empty(image),
        "Image validation failed: the image is empty."
    );
}

TEST_CASE("Channel validation accepts the expected count", "[unit][core][validation]") {
    const cv::Mat grayscale_image(2, 2, CV_8UC1, cv::Scalar{0});
    const cv::Mat color_image(2, 2, CV_8UC3, cv::Scalar{0, 0, 0});

    REQUIRE_NOTHROW(pdi::core::ImageValidator::require_channels(grayscale_image, 1));
    REQUIRE_NOTHROW(pdi::core::ImageValidator::require_channels(color_image, 3));
}

TEST_CASE("Channel validation reports expected and actual counts", "[unit][core][validation]") {
    const cv::Mat image(2, 2, CV_8UC3, cv::Scalar{0, 0, 0});

    REQUIRE_THROWS_WITH(
        pdi::core::ImageValidator::require_channels(image, 1),
        "Image validation failed: expected 1 channel(s), but received 3."
    );
}

TEST_CASE("Channel validation rejects non-positive expectations", "[unit][core][validation]") {
    const cv::Mat image(2, 2, CV_8UC1, cv::Scalar{0});

    REQUIRE_THROWS_WITH(
        pdi::core::ImageValidator::require_channels(image, 0),
        "Image validation failed: expected channel count must be positive."
    );
}

TEST_CASE("Depth validation accepts unsigned 8-bit images", "[unit][core][validation]") {
    const cv::Mat image(2, 2, CV_8UC3, cv::Scalar{0, 0, 0});

    REQUIRE_NOTHROW(pdi::core::ImageValidator::require_depth_8u(image));
}

TEST_CASE("Depth validation rejects other depths", "[unit][core][validation]") {
    const cv::Mat image(2, 2, CV_32FC1, cv::Scalar{0.0F});

    REQUIRE_THROWS_WITH(
        pdi::core::ImageValidator::require_depth_8u(image),
        "Image validation failed: expected CV_8U depth, but received depth code 5."
    );
}

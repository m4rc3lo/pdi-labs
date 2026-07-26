/**
 * @file manual_threshold_test.cpp
 * @brief Tests manual global and interval thresholding.
 */

#include "pdi/segmentation/manual_threshold.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "ManualThreshold applies inclusive global boundary",
    "[unit][m2-1][threshold]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(2, 4)
            << 0, 99, 100, 101,
               127, 128, 254, 255
    );

    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(2, 4)
            << 0, 0, 255, 255,
               255, 255, 255, 255
    );

    const cv::Mat actual =
        pdi::segmentation::ManualThreshold{}.binary_global(
            input,
            100
        );

    REQUIRE(actual.type() == CV_8UC1);
    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "ManualThreshold handles global limits zero and 255",
    "[unit][m2-1][threshold][boundary]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(1, 3) << 0, 1, 255
    );

    const auto all_foreground =
        pdi::segmentation::ManualThreshold{}.binary_global(input, 0);
    const auto maximum_only =
        pdi::segmentation::ManualThreshold{}.binary_global(input, 255);

    const cv::Mat expected_all = (
        cv::Mat_<std::uint8_t>(1, 3) << 255, 255, 255
    );
    const cv::Mat expected_maximum = (
        cv::Mat_<std::uint8_t>(1, 3) << 0, 0, 255
    );

    pdi::testing::require_mat_exact(all_foreground, expected_all);
    pdi::testing::require_mat_exact(maximum_only, expected_maximum);
}

TEST_CASE(
    "ManualThreshold selects inclusive interval boundaries",
    "[unit][m2-1][threshold][interval]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(2, 5)
            << 0, 49, 50, 51, 100,
               149, 150, 151, 200, 255
    );

    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(2, 5)
            << 0, 0, 255, 255, 255,
               255, 255, 0, 0, 0
    );

    const cv::Mat actual =
        pdi::segmentation::ManualThreshold{}.select_interval(
            input,
            50,
            150
        );

    REQUIRE(actual.type() == CV_8UC1);
    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "ManualThreshold accepts single-value interval",
    "[unit][m2-1][threshold][boundary]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(1, 4) << 41, 42, 42, 43
    );

    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(1, 4) << 0, 255, 255, 0
    );

    const cv::Mat actual =
        pdi::segmentation::ManualThreshold{}.select_interval(
            input,
            42,
            42
        );

    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "ManualThreshold rejects reversed interval",
    "[unit][m2-1][threshold][validation]"
) {
    const cv::Mat input(1, 1, CV_8UC1, cv::Scalar{100});

    REQUIRE_THROWS_WITH(
        pdi::segmentation::ManualThreshold{}.select_interval(
            input,
            151,
            150
        ),
        Catch::Matchers::ContainsSubstring(
            "minimum value must not exceed maximum value"
        )
    );
}

TEST_CASE(
    "ManualThreshold rejects unsupported input type",
    "[unit][m2-1][threshold][validation]"
) {
    const cv::Mat color_image(2, 2, CV_8UC3, cv::Scalar{0, 0, 0});

    REQUIRE_THROWS(
        pdi::segmentation::ManualThreshold{}.binary_global(
            color_image,
            128
        )
    );

    const cv::Mat floating_image(2, 2, CV_32FC1, cv::Scalar{0.0F});

    REQUIRE_THROWS(
        pdi::segmentation::ManualThreshold{}.select_interval(
            floating_image,
            0,
            255
        )
    );
}

/**
 * @file smoothing_filters_test.cpp
 * @brief Tests smoothing filters built on SpatialConvolution.
 */

#include "pdi/spatial/smoothing_filters.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "SmoothingFilters preserves constant regions",
    "[unit][m1-3][smoothing]"
) {
    const cv::Mat input(7, 7, CV_8UC1, cv::Scalar{120});
    const pdi::spatial::SmoothingFilters filters;
    const auto border = pdi::spatial::BorderStrategy::ReplicateBorder;

    pdi::testing::require_mat_exact(
        filters.mean_3x3(input, border),
        input
    );
    pdi::testing::require_mat_exact(
        filters.weighted_mean_3x3(input, border),
        input
    );
    pdi::testing::require_mat_exact(
        filters.mean_5x5(input, border),
        input
    );
}

TEST_CASE(
    "SmoothingFilters produces expected impulse responses",
    "[unit][m1-3][smoothing]"
) {
    cv::Mat input = cv::Mat::zeros(7, 7, CV_8UC1);
    input.ptr<std::uint8_t>(3)[3] = 255;

    const pdi::spatial::SmoothingFilters filters;
    const auto border = pdi::spatial::BorderStrategy::CopyBorder;

    const cv::Mat mean_3x3 = filters.mean_3x3(input, border);
    const cv::Mat weighted_3x3 =
        filters.weighted_mean_3x3(input, border);
    const cv::Mat mean_5x5 = filters.mean_5x5(input, border);

    REQUIRE(mean_3x3.ptr<std::uint8_t>(3)[3] == 28);
    REQUIRE(weighted_3x3.ptr<std::uint8_t>(3)[3] == 64);
    REQUIRE(mean_5x5.ptr<std::uint8_t>(3)[3] == 10);

    REQUIRE(mean_3x3.ptr<std::uint8_t>(3)[2] == 28);
    REQUIRE(weighted_3x3.ptr<std::uint8_t>(3)[2] == 32);
    REQUIRE(mean_5x5.ptr<std::uint8_t>(3)[2] == 10);
}

TEST_CASE(
    "Weighted mean retains more central detail than uniform means",
    "[unit][m1-3][smoothing]"
) {
    cv::Mat input = cv::Mat::zeros(7, 7, CV_8UC1);
    input.ptr<std::uint8_t>(3)[3] = 255;

    const pdi::spatial::SmoothingFilters filters;

    const cv::Mat mean_3x3 = filters.mean_3x3(input);
    const cv::Mat weighted_3x3 = filters.weighted_mean_3x3(input);
    const cv::Mat mean_5x5 = filters.mean_5x5(input);

    REQUIRE(
        weighted_3x3.ptr<std::uint8_t>(3)[3]
        > mean_3x3.ptr<std::uint8_t>(3)[3]
    );
    REQUIRE(
        mean_3x3.ptr<std::uint8_t>(3)[3]
        > mean_5x5.ptr<std::uint8_t>(3)[3]
    );
}

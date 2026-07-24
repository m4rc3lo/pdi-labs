/**
 * @file channel_separator_test.cpp
 * @brief Tests exact manual BGR channel separation.
 */

#include "pdi/value/channel_separator.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE("ChannelSeparator separates a 2x2 BGR image exactly", "[unit][m1-1][channels]") {
    cv::Mat input(2, 2, CV_8UC3);

    auto* row0 = input.ptr<cv::Vec3b>(0);
    row0[0] = cv::Vec3b{10, 20, 30};
    row0[1] = cv::Vec3b{40, 50, 60};

    auto* row1 = input.ptr<cv::Vec3b>(1);
    row1[0] = cv::Vec3b{70, 80, 90};
    row1[1] = cv::Vec3b{100, 110, 120};

    const pdi::value::ChannelSet result =
        pdi::value::ChannelSeparator{}.separate(input);

    cv::Mat expected_blue(2, 2, CV_8UC1);
    cv::Mat expected_green(2, 2, CV_8UC1);
    cv::Mat expected_red(2, 2, CV_8UC1);

    auto* blue0 = expected_blue.ptr<std::uint8_t>(0);
    auto* blue1 = expected_blue.ptr<std::uint8_t>(1);
    blue0[0] = 10;
    blue0[1] = 40;
    blue1[0] = 70;
    blue1[1] = 100;

    auto* green0 = expected_green.ptr<std::uint8_t>(0);
    auto* green1 = expected_green.ptr<std::uint8_t>(1);
    green0[0] = 20;
    green0[1] = 50;
    green1[0] = 80;
    green1[1] = 110;

    auto* red0 = expected_red.ptr<std::uint8_t>(0);
    auto* red1 = expected_red.ptr<std::uint8_t>(1);
    red0[0] = 30;
    red0[1] = 60;
    red1[0] = 90;
    red1[1] = 120;

    pdi::testing::require_mat_exact(result.blue, expected_blue);
    pdi::testing::require_mat_exact(result.green, expected_green);
    pdi::testing::require_mat_exact(result.red, expected_red);
}

TEST_CASE("ChannelSeparator rejects non-BGR input", "[unit][m1-1][channels]") {
    const cv::Mat grayscale(1, 1, CV_8UC1, cv::Scalar{0});

    REQUIRE_THROWS_WITH(
        pdi::value::ChannelSeparator{}.separate(grayscale),
        "Image validation failed: expected 3 channel(s), but received 1."
    );
}

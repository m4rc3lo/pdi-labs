/**
 * @file grayscale_converter_test.cpp
 * @brief Tests exact manual BGR-to-grayscale conversions.
 */

#include "pdi/value/grayscale_converter.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "GrayscaleConverter computes exact arithmetic means",
    "[unit][m1-1][grayscale]"
) {
    cv::Mat input(2, 2, CV_8UC3);

    auto* row0 = input.ptr<cv::Vec3b>(0);
    row0[0] = cv::Vec3b{0, 0, 0};
    row0[1] = cv::Vec3b{255, 255, 255};

    auto* row1 = input.ptr<cv::Vec3b>(1);
    row1[0] = cv::Vec3b{0, 0, 255};
    row1[1] = cv::Vec3b{10, 20, 31};

    cv::Mat expected(2, 2, CV_8UC1);
    auto* expected0 = expected.ptr<std::uint8_t>(0);
    expected0[0] = 0;
    expected0[1] = 255;

    auto* expected1 = expected.ptr<std::uint8_t>(1);
    expected1[0] = 85;
    expected1[1] = 20;

    const cv::Mat result =
        pdi::value::GrayscaleConverter{}.convert_average(input);

    pdi::testing::require_mat_exact(result, expected);
}

TEST_CASE(
    "GrayscaleConverter computes exact weighted intensities",
    "[unit][m1-1][grayscale]"
) {
    cv::Mat input(2, 2, CV_8UC3);

    auto* row0 = input.ptr<cv::Vec3b>(0);
    row0[0] = cv::Vec3b{255, 0, 0};
    row0[1] = cv::Vec3b{0, 255, 0};

    auto* row1 = input.ptr<cv::Vec3b>(1);
    row1[0] = cv::Vec3b{0, 0, 255};
    row1[1] = cv::Vec3b{10, 20, 30};

    cv::Mat expected(2, 2, CV_8UC1);
    auto* expected0 = expected.ptr<std::uint8_t>(0);
    expected0[0] = 29;
    expected0[1] = 150;

    auto* expected1 = expected.ptr<std::uint8_t>(1);
    expected1[0] = 76;
    expected1[1] = 22;

    const cv::Mat result =
        pdi::value::GrayscaleConverter{}.convert_weighted(input);

    pdi::testing::require_mat_exact(result, expected);
}

TEST_CASE(
    "GrayscaleConverter rejects non-BGR input",
    "[unit][m1-1][grayscale]"
) {
    const cv::Mat grayscale(1, 1, CV_8UC1, cv::Scalar{0});
    const pdi::value::GrayscaleConverter converter;

    REQUIRE_THROWS_WITH(
        converter.convert_average(grayscale),
        "Image validation failed: expected 3 channel(s), but received 1."
    );

    REQUIRE_THROWS_WITH(
        converter.convert_weighted(grayscale),
        "Image validation failed: expected 3 channel(s), but received 1."
    );
}

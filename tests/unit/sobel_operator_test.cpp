/**
 * @file sobel_operator_test.cpp
 * @brief Tests the manual Sobel operator.
 */

#include "pdi/spatial/sobel_operator.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "SobelOperator computes exact Gx for a vertical intensity transition",
    "[unit][m1-3][sobel]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 0, 10, 20,
               0, 10, 20,
               0, 10, 20
    );

    const auto result = pdi::spatial::SobelOperator{}.apply(
        input,
        pdi::spatial::BorderStrategy::CopyBorder
    );

    REQUIRE(result.gradient_x.type() == CV_64FC1);
    REQUIRE(result.gradient_y.type() == CV_64FC1);
    REQUIRE(result.gradient_x.ptr<double>(1)[1] == 80.0);
    REQUIRE(result.gradient_y.ptr<double>(1)[1] == 0.0);
    REQUIRE(result.magnitude_approximate.ptr<double>(1)[1] == 80.0);
    REQUIRE(result.magnitude_euclidean.ptr<double>(1)[1] == 80.0);
}

TEST_CASE(
    "SobelOperator computes exact Gy for a horizontal intensity transition",
    "[unit][m1-3][sobel]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 0, 0, 0,
               10, 10, 10,
               20, 20, 20
    );

    const auto result = pdi::spatial::SobelOperator{}.apply(
        input,
        pdi::spatial::BorderStrategy::CopyBorder
    );

    REQUIRE(result.gradient_x.ptr<double>(1)[1] == 0.0);
    REQUIRE(result.gradient_y.ptr<double>(1)[1] == 80.0);
    REQUIRE(result.magnitude_approximate.ptr<double>(1)[1] == 80.0);
    REQUIRE(result.magnitude_euclidean.ptr<double>(1)[1] == 80.0);
}

TEST_CASE(
    "SobelOperator preserves gradient sign",
    "[unit][m1-3][sobel]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 20, 10, 0,
               20, 10, 0,
               20, 10, 0
    );

    const auto result = pdi::spatial::SobelOperator{}.apply(
        input,
        pdi::spatial::BorderStrategy::CopyBorder
    );

    REQUIRE(result.gradient_x.ptr<double>(1)[1] == -80.0);
    REQUIRE(result.magnitude_approximate.ptr<double>(1)[1] == 80.0);
}

TEST_CASE(
    "SobelOperator compares approximate and Euclidean magnitudes",
    "[unit][m1-3][sobel]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 0, 10, 20,
               10, 20, 30,
               20, 30, 40
    );

    const auto result = pdi::spatial::SobelOperator{}.apply(
        input,
        pdi::spatial::BorderStrategy::CopyBorder
    );

    REQUIRE(result.gradient_x.ptr<double>(1)[1] == 80.0);
    REQUIRE(result.gradient_y.ptr<double>(1)[1] == 80.0);
    REQUIRE(result.magnitude_approximate.ptr<double>(1)[1] == 160.0);
    REQUIRE(
        result.magnitude_euclidean.ptr<double>(1)[1]
        == Catch::Approx(std::sqrt(12800.0))
    );
    REQUIRE(
        result.magnitude_approximate.ptr<double>(1)[1]
        > result.magnitude_euclidean.ptr<double>(1)[1]
    );
}

TEST_CASE(
    "SobelOperator creates byte visualizations",
    "[unit][m1-3][sobel]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 20, 10, 0,
               20, 10, 0,
               20, 10, 0
    );

    const auto result = pdi::spatial::SobelOperator{}.apply(input);

    REQUIRE(result.gradient_x_visualization.type() == CV_8UC1);
    REQUIRE(result.gradient_y_visualization.type() == CV_8UC1);
    REQUIRE(
        result.magnitude_approximate_visualization.type()
        == CV_8UC1
    );
    REQUIRE(
        result.magnitude_euclidean_visualization.type()
        == CV_8UC1
    );
}

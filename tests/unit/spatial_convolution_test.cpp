/**
 * @file spatial_convolution_test.cpp
 * @brief Tests generic manual spatial convolution.
 */

#include "pdi/spatial/spatial_convolution.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "SpatialConvolution preserves the interior with an identity kernel",
    "[unit][m1-3][convolution]"
) {
    cv::Mat input(3, 3, CV_8UC1);

    for (int row = 0; row < input.rows; ++row) {
        auto* row_ptr = input.ptr<std::uint8_t>(row);

        for (int col = 0; col < input.cols; ++col) {
            row_ptr[col] = static_cast<std::uint8_t>(row * 3 + col + 1);
        }
    }

    const cv::Mat kernel = (
        cv::Mat_<float>(3, 3)
            << 0.0F, 0.0F, 0.0F,
               0.0F, 1.0F, 0.0F,
               0.0F, 0.0F, 0.0F
    );

    cv::Mat expected = cv::Mat::zeros(3, 3, CV_8UC1);
    expected.ptr<std::uint8_t>(1)[1] = 5;

    const cv::Mat result =
        pdi::spatial::SpatialConvolution{}.convolution(input, kernel);

    pdi::testing::require_mat_exact(result, expected);
}

TEST_CASE(
    "SpatialConvolution applies an asymmetric kernel without rotation",
    "[unit][m1-3][convolution]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 1, 2, 3,
               4, 5, 6,
               7, 8, 9
    );

    const cv::Mat kernel = (
        cv::Mat_<double>(3, 3)
            << 1.0, 0.0, 0.0,
               0.0, 0.0, 0.0,
               0.0, 0.0, 0.0
    );

    cv::Mat expected = cv::Mat::zeros(3, 3, CV_8UC1);
    expected.ptr<std::uint8_t>(1)[1] = 1;

    const cv::Mat result =
        pdi::spatial::SpatialConvolution{}.convolution(input, kernel);

    pdi::testing::require_mat_exact(result, expected);
}

TEST_CASE(
    "SpatialConvolution optionally normalizes by the kernel sum",
    "[unit][m1-3][convolution]"
) {
    const cv::Mat input(3, 3, CV_8UC1, cv::Scalar{90});
    const cv::Mat kernel(3, 3, CV_32FC1, cv::Scalar{1.0F});

    cv::Mat expected = cv::Mat::zeros(3, 3, CV_8UC1);
    expected.ptr<std::uint8_t>(1)[1] = 90;

    const cv::Mat result =
        pdi::spatial::SpatialConvolution{}.convolution(
            input,
            kernel,
            true
        );

    pdi::testing::require_mat_exact(result, expected);
}

TEST_CASE(
    "SpatialConvolution processes only complete neighborhoods",
    "[unit][m1-3][convolution]"
) {
    const cv::Mat input(5, 5, CV_8UC1, cv::Scalar{10});
    const cv::Mat kernel = (
        cv::Mat_<float>(3, 3)
            << 0.0F, 0.0F, 0.0F,
               0.0F, 1.0F, 0.0F,
               0.0F, 0.0F, 0.0F
    );

    const cv::Mat result =
        pdi::spatial::SpatialConvolution{}.convolution(input, kernel);

    for (int row = 0; row < result.rows; ++row) {
        const auto* row_ptr = result.ptr<std::uint8_t>(row);

        for (int col = 0; col < result.cols; ++col) {
            const bool is_border =
                row == 0 || row == result.rows - 1
                || col == 0 || col == result.cols - 1;

            REQUIRE(row_ptr[col] == (is_border ? 0 : 10));
        }
    }
}

TEST_CASE(
    "SpatialConvolution validates kernel shape and normalization",
    "[unit][m1-3][convolution]"
) {
    const cv::Mat input(5, 5, CV_8UC1, cv::Scalar{10});
    const pdi::spatial::SpatialConvolution convolution;

    const cv::Mat even_kernel(2, 2, CV_32FC1, cv::Scalar{1.0F});
    REQUIRE_THROWS_WITH(
        convolution.convolution(input, even_kernel),
        "Spatial convolution failed: kernel dimension must be odd."
    );

    const cv::Mat rectangular_kernel(3, 5, CV_32FC1, cv::Scalar{1.0F});
    REQUIRE_THROWS_WITH(
        convolution.convolution(input, rectangular_kernel),
        "Spatial convolution failed: kernel must be square."
    );

    const cv::Mat integer_kernel(3, 3, CV_8UC1, cv::Scalar{1});
    REQUIRE_THROWS_WITH(
        convolution.convolution(input, integer_kernel),
        "Spatial convolution failed: kernel must use CV_32F or CV_64F."
    );

    const cv::Mat zero_sum_kernel = (
        cv::Mat_<float>(3, 3)
            << 0.0F, 0.0F, 0.0F,
               0.0F, 1.0F, -1.0F,
               0.0F, 0.0F, 0.0F
    );
    REQUIRE_THROWS_WITH(
        convolution.convolution(input, zero_sum_kernel, true),
        "Spatial convolution failed: normalized kernel sum must not be zero."
    );
}

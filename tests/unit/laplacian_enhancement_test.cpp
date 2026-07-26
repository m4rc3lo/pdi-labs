/**
 * @file laplacian_enhancement_test.cpp
 * @brief Tests signed Laplacian response and enhancement.
 */

#include "pdi/spatial/laplacian_enhancement.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "Laplacian four-neighbor response preserves negative values",
    "[unit][m1-3][laplacian]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 0, 0, 0,
               0, 100, 0,
               0, 0, 0
    );

    const auto result =
        pdi::spatial::LaplacianEnhancement{}.apply(
            input,
            pdi::spatial::LaplacianKernel::FourNeighbor,
            1.0,
            pdi::spatial::BorderStrategy::ReplicateBorder
        );

    REQUIRE(result.raw_response.type() == CV_64FC1);
    REQUIRE(result.raw_response.ptr<double>(1)[1] == Catch::Approx(400.0));
    REQUIRE(result.raw_response.ptr<double>(0)[1] == Catch::Approx(-100.0));
    REQUIRE(result.raw_response.ptr<double>(1)[0] == Catch::Approx(-100.0));
}

TEST_CASE(
    "Laplacian eight-neighbor uses all surrounding pixels",
    "[unit][m1-3][laplacian]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 10, 10, 10,
               10, 20, 10,
               10, 10, 10
    );

    const auto result =
        pdi::spatial::LaplacianEnhancement{}.apply(
            input,
            pdi::spatial::LaplacianKernel::EightNeighbor,
            0.0,
            pdi::spatial::BorderStrategy::ReplicateBorder
        );

    REQUIRE(result.raw_response.ptr<double>(1)[1] == Catch::Approx(80.0));
    REQUIRE(result.enhanced_image.ptr<std::uint8_t>(1)[1] == 20);
}

TEST_CASE(
    "Laplacian enhancement applies factor before final saturation",
    "[unit][m1-3][laplacian]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 0, 0, 0,
               0, 100, 0,
               0, 0, 0
    );

    const auto half =
        pdi::spatial::LaplacianEnhancement{}.apply(
            input,
            pdi::spatial::LaplacianKernel::FourNeighbor,
            0.25
        );
    const auto full =
        pdi::spatial::LaplacianEnhancement{}.apply(
            input,
            pdi::spatial::LaplacianKernel::FourNeighbor,
            1.0
        );

    REQUIRE(half.enhanced_image.ptr<std::uint8_t>(1)[1] == 200);
    REQUIRE(full.enhanced_image.ptr<std::uint8_t>(1)[1] == 255);
    REQUIRE(full.raw_response.ptr<double>(0)[1] < 0.0);
}

TEST_CASE(
    "Laplacian visualization spans the byte range",
    "[unit][m1-3][laplacian]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 0, 0, 0,
               0, 100, 0,
               0, 0, 0
    );

    const auto result =
        pdi::spatial::LaplacianEnhancement{}.apply(
            input,
            pdi::spatial::LaplacianKernel::FourNeighbor,
            1.0
        );

    REQUIRE(result.response_visualization.type() == CV_8UC1);
    REQUIRE(result.response_visualization.ptr<std::uint8_t>(1)[1] == 255);
    REQUIRE(result.response_visualization.ptr<std::uint8_t>(0)[1] == 0);
}

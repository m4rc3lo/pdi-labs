#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

TEST_CASE("Integer matrices can be compared exactly", "[unit][mat][exact]") {
    const cv::Mat actual = (
        cv::Mat_<std::uint8_t>(2, 3)
        << 10, 20, 30,
           40, 50, 60
    );
    const cv::Mat expected = actual.clone();

    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE("Floating-point matrices can be compared with tolerance", "[unit][mat][near]") {
    const cv::Mat actual = (
        cv::Mat_<double>(2, 2)
        << 0.10, 0.20,
           0.30, 0.40
    );
    const cv::Mat expected = (
        cv::Mat_<double>(2, 2)
        << 0.11, 0.19,
           0.29, 0.41
    );

    pdi::testing::require_mat_near(actual, expected, 0.011);
}

/**
 * @file mask_operations_test.cpp
 * @brief Tests manual binary mask operations and applications.
 */

#include "pdi/segmentation/mask_operations.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

namespace {

[[nodiscard]] cv::Mat first_mask() {
    return (
        cv::Mat_<std::uint8_t>(2, 3)
            << 0, 255, 255,
               0, 0, 255
    );
}

[[nodiscard]] cv::Mat second_mask() {
    return (
        cv::Mat_<std::uint8_t>(2, 3)
            << 0, 0, 255,
               255, 0, 255
    );
}

} // namespace

TEST_CASE(
    "MaskOperations inverts a binary mask exactly",
    "[unit][m2-1][mask]"
) {
    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(2, 3)
            << 255, 0, 0,
               255, 255, 0
    );

    const cv::Mat actual =
        pdi::segmentation::MaskOperations{}.invert(first_mask());

    REQUIRE(actual.type() == CV_8UC1);
    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "MaskOperations computes intersection union and difference",
    "[unit][m2-1][mask][logical]"
) {
    const pdi::segmentation::MaskOperations operations;

    const cv::Mat expected_intersection = (
        cv::Mat_<std::uint8_t>(2, 3)
            << 0, 0, 255,
               0, 0, 255
    );
    const cv::Mat expected_union = (
        cv::Mat_<std::uint8_t>(2, 3)
            << 0, 255, 255,
               255, 0, 255
    );
    const cv::Mat expected_difference = (
        cv::Mat_<std::uint8_t>(2, 3)
            << 0, 255, 0,
               0, 0, 0
    );
    const cv::Mat expected_reverse_difference = (
        cv::Mat_<std::uint8_t>(2, 3)
            << 0, 0, 0,
               255, 0, 0
    );

    pdi::testing::require_mat_exact(
        operations.intersection(first_mask(), second_mask()),
        expected_intersection
    );
    pdi::testing::require_mat_exact(
        operations.union_of(first_mask(), second_mask()),
        expected_union
    );
    pdi::testing::require_mat_exact(
        operations.difference(first_mask(), second_mask()),
        expected_difference
    );
    pdi::testing::require_mat_exact(
        operations.difference(second_mask(), first_mask()),
        expected_reverse_difference
    );
}

TEST_CASE(
    "MaskOperations applies mask to grayscale image",
    "[unit][m2-1][mask][grayscale]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(2, 3)
            << 10, 20, 30,
               40, 50, 60
    );
    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(2, 3)
            << 0, 20, 30,
               0, 0, 60
    );

    const cv::Mat actual =
        pdi::segmentation::MaskOperations{}.apply_to_grayscale(
            input,
            first_mask()
        );

    REQUIRE(actual.type() == CV_8UC1);
    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "MaskOperations applies mask to BGR image",
    "[unit][m2-1][mask][color]"
) {
    const cv::Mat input = (
        cv::Mat_<cv::Vec3b>(1, 3)
            << cv::Vec3b{10, 20, 30},
               cv::Vec3b{40, 50, 60},
               cv::Vec3b{70, 80, 90}
    );
    const cv::Mat mask = (
        cv::Mat_<std::uint8_t>(1, 3) << 255, 0, 255
    );
    const cv::Mat expected = (
        cv::Mat_<cv::Vec3b>(1, 3)
            << cv::Vec3b{10, 20, 30},
               cv::Vec3b{0, 0, 0},
               cv::Vec3b{70, 80, 90}
    );

    const cv::Mat actual =
        pdi::segmentation::MaskOperations{}.apply_to_color(
            input,
            mask
        );

    REQUIRE(actual.type() == CV_8UC3);
    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "MaskOperations rejects nonbinary mask values",
    "[unit][m2-1][mask][validation]"
) {
    const cv::Mat invalid = (
        cv::Mat_<std::uint8_t>(1, 3) << 0, 128, 255
    );

    REQUIRE_THROWS_WITH(
        pdi::segmentation::MaskOperations{}.invert(invalid),
        Catch::Matchers::ContainsSubstring(
            "must contain only binary values 0 and 255"
        )
    );
}

TEST_CASE(
    "MaskOperations rejects incompatible mask dimensions",
    "[unit][m2-1][mask][validation]"
) {
    const cv::Mat small(1, 2, CV_8UC1, cv::Scalar{0});
    const cv::Mat large(2, 2, CV_8UC1, cv::Scalar{0});

    REQUIRE_THROWS_WITH(
        pdi::segmentation::MaskOperations{}.union_of(small, large),
        Catch::Matchers::ContainsSubstring(
            "matching dimensions"
        )
    );
}

TEST_CASE(
    "MaskOperations validates image type and dimensions",
    "[unit][m2-1][mask][validation]"
) {
    const cv::Mat mask(2, 2, CV_8UC1, cv::Scalar{255});
    const cv::Mat wrong_size(1, 2, CV_8UC1, cv::Scalar{10});
    const cv::Mat wrong_type(2, 2, CV_32FC1, cv::Scalar{10.0F});
    const cv::Mat color(2, 2, CV_8UC3, cv::Scalar{1, 2, 3});

    REQUIRE_THROWS(
        pdi::segmentation::MaskOperations{}.apply_to_grayscale(
            wrong_size,
            mask
        )
    );
    REQUIRE_THROWS(
        pdi::segmentation::MaskOperations{}.apply_to_grayscale(
            wrong_type,
            mask
        )
    );
    REQUIRE_THROWS(
        pdi::segmentation::MaskOperations{}.apply_to_grayscale(
            color,
            mask
        )
    );
    REQUIRE_THROWS(
        pdi::segmentation::MaskOperations{}.apply_to_color(
            wrong_size,
            mask
        )
    );
}

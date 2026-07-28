/**
 * @file binary_morphology_test.cpp
 * @brief Tests exact manual binary erosion and dilation.
 */

#include "pdi/morphology/binary_morphology.hpp"
#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cstdint>
#include <stdexcept>

TEST_CASE(
    "BinaryMorphology erodes a full square with outside background",
    "[unit][morphology][erosion]"
) {
    const cv::Mat input = cv::Mat(5, 5, CV_8UC1, cv::Scalar(255));

    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(5, 5)
        << 0, 0, 0, 0, 0,
           0, 255, 255, 255, 0,
           0, 255, 255, 255, 0,
           0, 255, 255, 255, 0,
           0, 0, 0, 0, 0
    );

    const cv::Mat actual = pdi::morphology::BinaryMorphology{}.erode(
        input,
        pdi::morphology::BinaryStructuringElement::square_3x3()
    );

    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "BinaryMorphology dilates one pixel with square element",
    "[unit][morphology][dilation]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(5, 5)
        << 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0,
           0, 0, 255, 0, 0,
           0, 0, 0, 0, 0,
           0, 0, 0, 0, 0
    );

    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(5, 5)
        << 0, 0, 0, 0, 0,
           0, 255, 255, 255, 0,
           0, 255, 255, 255, 0,
           0, 255, 255, 255, 0,
           0, 0, 0, 0, 0
    );

    const cv::Mat actual = pdi::morphology::BinaryMorphology{}.dilate(
        input,
        pdi::morphology::BinaryStructuringElement::square_3x3()
    );

    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "BinaryMorphology supports cross element independently of GUI",
    "[unit][morphology][structuring-element]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
        << 0, 0, 0,
           0, 255, 0,
           0, 0, 0
    );

    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(3, 3)
        << 0, 255, 0,
           255, 255, 255,
           0, 255, 0
    );

    const cv::Mat actual = pdi::morphology::BinaryMorphology{}.dilate(
        input,
        pdi::morphology::BinaryStructuringElement::cross_3x3()
    );

    pdi::testing::require_mat_exact(actual, expected);
}

TEST_CASE(
    "BinaryMorphology rejects non-binary input",
    "[unit][morphology][validation]"
) {
    const cv::Mat invalid = (
        cv::Mat_<std::uint8_t>(2, 2)
        << 0, 255,
           127, 0
    );

    REQUIRE_THROWS_AS(
        pdi::morphology::BinaryMorphology{}.erode(
            invalid,
            pdi::morphology::BinaryStructuringElement::square_3x3()
        ),
        std::invalid_argument
    );
}

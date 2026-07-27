/**
 * @file connected_component_labeler_test.cpp
 * @brief Tests manual connected-component labeling.
 */

#include "pdi/segmentation/connected_component_labeler.hpp"
#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cstdint>
#include <stdexcept>

namespace {

cv::Mat make_diagonal_binary_image() {
    return (cv::Mat_<std::uint8_t>(3, 3) <<
        255, 0, 0,
        0, 255, 0,
        0, 0, 255);
}

} // namespace

TEST_CASE("Four-connectivity keeps diagonal pixels separated") {
    const cv::Mat input = make_diagonal_binary_image();

    const auto result =
        pdi::segmentation::ConnectedComponentLabeler{}.label(
            input,
            pdi::segmentation::Connectivity::Four
        );

    const cv::Mat expected = (cv::Mat_<std::int32_t>(3, 3) <<
        1, 0, 0,
        0, 2, 0,
        0, 0, 3);

    REQUIRE(result.component_count == 3);
    REQUIRE(result.labels.type() == CV_32SC1);
    pdi::testing::require_mat_exact(result.labels, expected);
}

TEST_CASE("Eight-connectivity joins diagonal pixels") {
    const cv::Mat input = make_diagonal_binary_image();

    const auto result =
        pdi::segmentation::ConnectedComponentLabeler{}.label(
            input,
            pdi::segmentation::Connectivity::Eight
        );

    const cv::Mat expected = (cv::Mat_<std::int32_t>(3, 3) <<
        1, 0, 0,
        0, 1, 0,
        0, 0, 1);

    REQUIRE(result.component_count == 1);
    pdi::testing::require_mat_exact(result.labels, expected);
}

TEST_CASE("Background keeps label zero and foreground labels are consecutive") {
    const cv::Mat input = (cv::Mat_<std::uint8_t>(4, 5) <<
        255, 255, 0, 0, 0,
        255, 0, 0, 255, 255,
        0, 0, 0, 255, 0,
        255, 0, 0, 0, 0);

    const auto result =
        pdi::segmentation::ConnectedComponentLabeler{}.label(
            input,
            pdi::segmentation::Connectivity::Four
        );

    const cv::Mat expected = (cv::Mat_<std::int32_t>(4, 5) <<
        1, 1, 0, 0, 0,
        1, 0, 0, 2, 2,
        0, 0, 0, 2, 0,
        3, 0, 0, 0, 0);

    REQUIRE(result.component_count == 3);
    pdi::testing::require_mat_exact(result.labels, expected);
}

TEST_CASE("Component selection returns a strict binary mask") {
    const cv::Mat labels = (cv::Mat_<std::int32_t>(2, 4) <<
        0, 1, 1, 0,
        2, 0, 1, 2);

    const cv::Mat mask =
        pdi::segmentation::ConnectedComponentLabeler{}.select_component(
            labels,
            2
        );

    const cv::Mat expected = (cv::Mat_<std::uint8_t>(2, 4) <<
        0, 0, 0, 0,
        255, 0, 0, 255);

    pdi::testing::require_mat_exact(mask, expected);
}

TEST_CASE("Labeling rejects non-binary values") {
    const cv::Mat invalid = (cv::Mat_<std::uint8_t>(1, 3) << 0, 127, 255);

    REQUIRE_THROWS_AS(
        pdi::segmentation::ConnectedComponentLabeler{}.label(
            invalid,
            pdi::segmentation::Connectivity::Four
        ),
        std::invalid_argument
    );
}

TEST_CASE("Labeling rejects unsupported image types") {
    const cv::Mat invalid = cv::Mat::zeros(2, 2, CV_8UC3);

    REQUIRE_THROWS_AS(
        pdi::segmentation::ConnectedComponentLabeler{}.label(
            invalid,
            pdi::segmentation::Connectivity::Eight
        ),
        std::invalid_argument
    );
}

TEST_CASE("Labeling rejects unsupported connectivity") {
    const cv::Mat input = cv::Mat::zeros(2, 2, CV_8UC1);

    REQUIRE_THROWS_AS(
        pdi::segmentation::ConnectedComponentLabeler{}.label(
            input,
            static_cast<pdi::segmentation::Connectivity>(6)
        ),
        std::invalid_argument
    );
}

TEST_CASE("Component selection requires a positive label") {
    const cv::Mat labels = cv::Mat::zeros(2, 2, CV_32SC1);

    REQUIRE_THROWS_AS(
        pdi::segmentation::ConnectedComponentLabeler{}.select_component(
            labels,
            0
        ),
        std::invalid_argument
    );
}

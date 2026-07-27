/**
 * @file label_visualizer_test.cpp
 * @brief Tests label visualization and optional OpenCV validation.
 */

#include "pdi/segmentation/label_visualizer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

TEST_CASE(
    "LabelVisualizer colorizes labels deterministically",
    "[unit][segmentation][labels][visualization]"
) {
    const cv::Mat labels = (
        cv::Mat_<int>(2, 3)
        << 0, 1, 2,
           2, 1, 0
    );

    const cv::Mat colored =
        pdi::segmentation::LabelVisualizer{}.colorize(labels);

    REQUIRE(colored.type() == CV_8UC3);
    REQUIRE(colored.rows == labels.rows);
    REQUIRE(colored.cols == labels.cols);

    REQUIRE(colored.ptr<cv::Vec3b>(0)[0] == cv::Vec3b(0, 0, 0));
    REQUIRE(colored.ptr<cv::Vec3b>(0)[1] != cv::Vec3b(0, 0, 0));
    REQUIRE(colored.ptr<cv::Vec3b>(0)[2] != cv::Vec3b(0, 0, 0));
    REQUIRE(colored.ptr<cv::Vec3b>(0)[1] != colored.ptr<cv::Vec3b>(0)[2]);
    REQUIRE(
        colored.ptr<cv::Vec3b>(0)[1] == colored.ptr<cv::Vec3b>(1)[1]
    );
}

TEST_CASE(
    "LabelVisualizer normalizes first occurrence order",
    "[unit][segmentation][labels][visualization]"
) {
    const cv::Mat labels = (
        cv::Mat_<int>(2, 4)
        << 5, 5, 0, 9,
           9, 0, 2, 2
    );

    const cv::Mat expected = (
        cv::Mat_<int>(2, 4)
        << 1, 1, 0, 2,
           2, 0, 3, 3
    );

    const cv::Mat normalized =
        pdi::segmentation::LabelVisualizer{}.normalize_labels(labels);

    for (int row = 0; row < normalized.rows; ++row) {
        const auto* actual_row = normalized.ptr<int>(row);
        const auto* expected_row = expected.ptr<int>(row);

        for (int column = 0; column < normalized.cols; ++column) {
            REQUIRE(actual_row[column] == expected_row[column]);
        }
    }
}

TEST_CASE(
    "LabelVisualizer optionally compares with OpenCV",
    "[unit][segmentation][labels][validation]"
) {
    const cv::Mat binary = (
        cv::Mat_<std::uint8_t>(3, 3)
        << 255, 0, 0,
           0, 255, 255,
           0, 0, 0
    );

    const auto labeling =
        pdi::segmentation::ConnectedComponentLabeler{}.label(
            binary,
            pdi::segmentation::Connectivity::Eight
        );

    const auto summary =
        pdi::segmentation::LabelVisualizer{}.compare_with_opencv(
            binary,
            labeling.labels,
            pdi::segmentation::Connectivity::Eight
        );

    REQUIRE(summary.compared_with_opencv);
    REQUIRE(summary.manual_component_count == 1);
    REQUIRE(summary.opencv_component_count == 1);
    REQUIRE(summary.same_partition);
}

/**
 * @file library_segmentation_pipeline_test.cpp
 * @brief Tests basic integration of library-based segmentation examples.
 */

#include "pdi/segmentation/library_segmentation_pipeline.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <cstdint>
#include <string>

TEST_CASE(
    "Library segmentation converts BGR to HSV and exposes channels",
    "[integration][m2-1][library][hsv]"
) {
    const cv::Mat input(
        2,
        2,
        CV_8UC3,
        cv::Scalar{10, 120, 240}
    );

    const auto result =
        pdi::segmentation::LibrarySegmentationPipeline{}.run(
            input,
            {
                .operation =
                    pdi::segmentation::LibrarySegmentationOperation::
                        BgrToHsv,
            }
        );

    REQUIRE(result.operation_name == "bgr_to_hsv");
    REQUIRE(result.visual_outputs.size() == 4);
    REQUIRE(result.numeric_artifacts.size() == 1);
    REQUIRE(result.numeric_artifacts[0].value.type() == CV_8UC3);
}

TEST_CASE(
    "Library segmentation Otsu returns a binary image and threshold",
    "[integration][m2-1][library][otsu]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(2, 4)
            << 10, 20, 30, 40,
               200, 210, 220, 230
    );

    const auto result =
        pdi::segmentation::LibrarySegmentationPipeline{}.run(
            input,
            {
                .operation =
                    pdi::segmentation::LibrarySegmentationOperation::
                        Otsu,
            }
        );

    REQUIRE(result.visual_outputs.size() == 1);
    REQUIRE(result.visual_outputs[0].image.type() == CV_8UC1);
    REQUIRE_FALSE(result.parameters.empty());
}

TEST_CASE(
    "Library segmentation validates adaptive block size",
    "[integration][m2-1][library][adaptive]"
) {
    const cv::Mat input(5, 5, CV_8UC1, cv::Scalar{100});

    REQUIRE_THROWS(
        pdi::segmentation::LibrarySegmentationPipeline{}.run(
            input,
            {
                .operation =
                    pdi::segmentation::LibrarySegmentationOperation::
                        AdaptiveMean,
                .adaptive_block_size = 4,
            }
        )
    );
}

TEST_CASE(
    "Library segmentation preserves raw distance matrix",
    "[integration][m2-1][library][distance]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(5, 5)
            << 0, 0, 0, 0, 0,
               0, 255, 255, 255, 0,
               0, 255, 255, 255, 0,
               0, 255, 255, 255, 0,
               0, 0, 0, 0, 0
    );

    const auto result =
        pdi::segmentation::LibrarySegmentationPipeline{}.run(
            input,
            {
                .operation =
                    pdi::segmentation::LibrarySegmentationOperation::
                        DistanceTransform,
            }
        );

    REQUIRE(result.numeric_artifacts.size() == 1);
    REQUIRE(result.numeric_artifacts[0].value.type() == CV_32FC1);
    REQUIRE(result.visual_outputs[0].image.type() == CV_8UC1);
}

TEST_CASE(
    "Library segmentation Watershed exposes explicit stages",
    "[integration][m2-1][library][watershed]"
) {
    cv::Mat input(40, 40, CV_8UC3, cv::Scalar{255, 255, 255});
    cv::rectangle(
        input,
        cv::Point{5, 5},
        cv::Point{15, 30},
        cv::Scalar{0, 0, 0},
        cv::FILLED
    );
    cv::rectangle(
        input,
        cv::Point{24, 5},
        cv::Point{34, 30},
        cv::Scalar{0, 0, 0},
        cv::FILLED
    );

    const auto result =
        pdi::segmentation::LibrarySegmentationPipeline{}.run(
            input,
            {
                .operation =
                    pdi::segmentation::LibrarySegmentationOperation::
                        Watershed,
                .foreground_ratio = 0.4,
            }
        );

    REQUIRE(result.visual_outputs.size() == 7);
    REQUIRE(result.numeric_artifacts.size() == 3);
    REQUIRE(result.numeric_artifacts[0].value.type() == CV_32FC1);
    REQUIRE(result.numeric_artifacts[2].value.type() == CV_32SC1);
}

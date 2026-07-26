/**
 * @file m2_1_pipeline_test.cpp
 * @brief Tests the integrated Laboratory M2.1 pipeline.
 */

#include "pdi/labs/m2_1_pipeline.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "M21Pipeline runs manual global thresholding",
    "[integration][m2-1][pipeline][manual]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(1, 4) << 0, 127, 128, 255
    );
    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(1, 4) << 0, 0, 255, 255
    );

    const auto result = pdi::labs::M21Pipeline{}.run(
        input,
        cv::Mat{},
        {
            .operation = pdi::labs::M21Operation::ManualGlobal,
            .threshold = 128,
        }
    );

    REQUIRE(result.operation_name == "manual_global");
    REQUIRE(result.visual_outputs.size() == 1);
    REQUIRE(result.numeric_artifacts.empty());
    pdi::testing::require_mat_exact(
        result.visual_outputs[0].image,
        expected
    );
}

TEST_CASE(
    "M21Pipeline integrates binary mask union",
    "[integration][m2-1][pipeline][mask]"
) {
    const cv::Mat first = (
        cv::Mat_<std::uint8_t>(1, 3) << 255, 0, 0
    );
    const cv::Mat second = (
        cv::Mat_<std::uint8_t>(1, 3) << 0, 255, 0
    );
    const cv::Mat expected = (
        cv::Mat_<std::uint8_t>(1, 3) << 255, 255, 0
    );

    const auto result = pdi::labs::M21Pipeline{}.run(
        first,
        second,
        {.operation = pdi::labs::M21Operation::MaskUnion}
    );

    REQUIRE(result.operation_name == "mask_union");
    pdi::testing::require_mat_exact(
        result.visual_outputs[0].image,
        expected
    );
}

TEST_CASE(
    "M21Pipeline applies a mask to color image",
    "[integration][m2-1][pipeline][mask][color]"
) {
    const cv::Mat input = (
        cv::Mat_<cv::Vec3b>(1, 2)
            << cv::Vec3b{10, 20, 30}, cv::Vec3b{40, 50, 60}
    );
    const cv::Mat mask = (
        cv::Mat_<std::uint8_t>(1, 2) << 255, 0
    );
    const cv::Mat expected = (
        cv::Mat_<cv::Vec3b>(1, 2)
            << cv::Vec3b{10, 20, 30}, cv::Vec3b{0, 0, 0}
    );

    const auto result = pdi::labs::M21Pipeline{}.run(
        input,
        mask,
        {.operation = pdi::labs::M21Operation::MaskApplyColor}
    );

    pdi::testing::require_mat_exact(
        result.visual_outputs[0].image,
        expected
    );
}

TEST_CASE(
    "M21Pipeline delegates distance transform and preserves raw matrix",
    "[integration][m2-1][pipeline][library][distance]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 0, 0, 0,
               0, 255, 0,
               0, 0, 0
    );

    const auto result = pdi::labs::M21Pipeline{}.run(
        input,
        cv::Mat{},
        {
            .operation = pdi::labs::M21Operation::DistanceTransform,
            .distance_mask_size = 3,
        }
    );

    REQUIRE(result.operation_name == "distance_transform");
    REQUIRE(result.visual_outputs.size() == 1);
    REQUIRE(result.visual_outputs[0].image.type() == CV_8UC1);
    REQUIRE(result.numeric_artifacts.size() == 1);
    REQUIRE(result.numeric_artifacts[0].value.type() == CV_32FC1);
}

TEST_CASE(
    "M21Pipeline declares input requirements",
    "[integration][m2-1][pipeline][requirements]"
) {
    REQUIRE(pdi::labs::M21Pipeline::requires_color_input(
        pdi::labs::M21Operation::Watershed
    ));
    REQUIRE(pdi::labs::M21Pipeline::requires_color_input(
        pdi::labs::M21Operation::MaskApplyColor
    ));
    REQUIRE_FALSE(pdi::labs::M21Pipeline::requires_color_input(
        pdi::labs::M21Operation::Otsu
    ));
    REQUIRE(pdi::labs::M21Pipeline::requires_secondary_input(
        pdi::labs::M21Operation::MaskDifference
    ));
    REQUIRE_FALSE(pdi::labs::M21Pipeline::requires_secondary_input(
        pdi::labs::M21Operation::ManualGlobal
    ));
}

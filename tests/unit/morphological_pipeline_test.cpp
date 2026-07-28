/**
 * @file morphological_pipeline_test.cpp
 * @brief Tests configurable morphology pipelines and traceability.
 */

#include "pdi/morphology/morphological_pipeline.hpp"
#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "MorphologicalPipeline removes isolated noise and records metrics",
    "[unit][morphology][pipeline][noise]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(7, 7)
        << 0, 0, 0, 0, 0, 0, 0,
           0, 255, 255, 255, 0, 0, 0,
           0, 255, 255, 255, 0, 255, 0,
           0, 255, 255, 255, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0
    );

    auto config =
        pdi::morphology::MorphologicalPipelineConfig::noise_removal_3x3();
    config.compare_with_opencv = true;

    const auto result =
        pdi::morphology::MorphologicalPipeline{}.run(input, config);

    REQUIRE(result.intermediate_images.size() == 2);
    REQUIRE(result.foreground_area_before == 10);
    REQUIRE(result.foreground_area_after == 9);
    REQUIRE(result.component_count_before == 2);
    REQUIRE(result.component_count_after == 1);
    REQUIRE(result.removed_component_count == 1);
    REQUIRE(result.removed_component_labels.size() == 1);
    REQUIRE(result.compared_with_opencv);
    REQUIRE(result.matches_opencv);
}

TEST_CASE(
    "MorphologicalPipeline fills a small hole",
    "[unit][morphology][pipeline][holes]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(7, 7)
        << 0, 0, 0, 0, 0, 0, 0,
           0, 255, 255, 255, 255, 255, 0,
           0, 255, 255, 255, 255, 255, 0,
           0, 255, 255, 0, 255, 255, 0,
           0, 255, 255, 255, 255, 255, 0,
           0, 255, 255, 255, 255, 255, 0,
           0, 0, 0, 0, 0, 0, 0
    );

    const auto config =
        pdi::morphology::MorphologicalPipelineConfig::hole_filling_3x3();

    const auto result =
        pdi::morphology::MorphologicalPipeline{}.run(input, config);

    REQUIRE(result.intermediate_images.size() == 2);
    REQUIRE(result.foreground_area_before == 24);
    REQUIRE(result.foreground_area_after == 25);
    REQUIRE(result.component_count_before == 1);
    REQUIRE(result.component_count_after == 1);
    REQUIRE(result.removed_component_count == 0);
    REQUIRE(result.output.ptr<std::uint8_t>(3)[3] == 255);
}

TEST_CASE(
    "MorphologicalPipeline exports sequence metrics and matrices",
    "[unit][morphology][pipeline][yaml]"
) {
    const cv::Mat input = cv::Mat(5, 5, CV_8UC1, cv::Scalar(0));
    auto config =
        pdi::morphology::MorphologicalPipelineConfig::noise_removal_3x3();

    const auto result =
        pdi::morphology::MorphologicalPipeline{}.run(input, config);

    const auto record =
        pdi::morphology::MorphologicalPipeline{}.make_processing_record(
            result,
            config,
            "0.6.0",
            "synthetic"
        );

    REQUIRE(record.laboratory == "M2.3");
    REQUIRE(record.operation == "morphological_pipeline");
    REQUIRE(record.parameters.size() >= 10);
    REQUIRE(record.numeric_artifacts.size() == 7);
    REQUIRE(record.numeric_artifacts[0].name == "structuring_element");
    REQUIRE(record.numeric_artifacts[2].name == "stage_1_erode");
    REQUIRE(record.numeric_artifacts[3].name == "stage_2_dilate");
    REQUIRE(record.numeric_artifacts[6].name == "final_labels");
}

TEST_CASE(
    "MorphologicalPipeline requires at least two operations",
    "[unit][morphology][pipeline][validation]"
) {
    pdi::morphology::MorphologicalPipelineConfig config{
        .element =
            pdi::morphology::BinaryStructuringElement::square_3x3(),
        .sequence = {
            pdi::morphology::MorphologicalOperation::Erode,
        },
    };

    REQUIRE_THROWS_AS(
        pdi::morphology::MorphologicalPipeline{}.run(
            cv::Mat(3, 3, CV_8UC1, cv::Scalar(0)),
            config
        ),
        std::invalid_argument
    );
}

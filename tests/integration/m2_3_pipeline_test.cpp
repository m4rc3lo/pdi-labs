/**
 * @file m2_3_pipeline_test.cpp
 * @brief Tests the integrated headless flow of Laboratory M2.3.
 */

#include "pdi/io/image_file_storage.hpp"
#include "pdi/io/processing_data_storage.hpp"
#include "pdi/morphology/morphological_pipeline.hpp"
#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cstdint>
#include <filesystem>

TEST_CASE(
    "Laboratory M2.3 writes visual and numeric artifacts",
    "[integration][m2.3]"
) {
    const auto directory =
        std::filesystem::temp_directory_path() / "pdi_labs_m2_3";
    std::filesystem::remove_all(directory);

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

    const pdi::morphology::MorphologicalPipeline pipeline;
    const auto result = pipeline.run(input, config);

    const pdi::io::ImageFileStorage image_storage;
    image_storage.save(directory / "input.png", result.input);
    image_storage.save(
        directory / "stage_1.png",
        result.intermediate_images[0]
    );
    image_storage.save(
        directory / "stage_2.png",
        result.intermediate_images[1]
    );
    image_storage.save(directory / "output.png", result.output);

    const auto record = pipeline.make_processing_record(
        result,
        config,
        "0.7.0",
        "synthetic"
    );
    const auto yaml_path = directory / "m2_3_result.yml";
    pdi::io::ProcessingDataStorage{}.save_yaml(yaml_path, record);

    REQUIRE(std::filesystem::exists(directory / "input.png"));
    REQUIRE(std::filesystem::exists(directory / "stage_1.png"));
    REQUIRE(std::filesystem::exists(directory / "stage_2.png"));
    REQUIRE(std::filesystem::exists(directory / "output.png"));
    REQUIRE(std::filesystem::exists(yaml_path));

    const auto loaded =
        pdi::io::ProcessingDataStorage{}.load_yaml(yaml_path);

    REQUIRE(loaded.project_version == "0.7.0");
    REQUIRE(loaded.laboratory == "M2.3");
    REQUIRE(loaded.operation == "morphological_pipeline");
    REQUIRE(loaded.numeric_artifacts.size() == 7);
    REQUIRE(loaded.numeric_artifacts[0].name == "structuring_element");
    REQUIRE(loaded.numeric_artifacts[2].name == "stage_1_erode");
    REQUIRE(loaded.numeric_artifacts[3].name == "stage_2_dilate");
    REQUIRE(loaded.numeric_artifacts[6].name == "final_labels");

    pdi::testing::require_mat_exact(
        loaded.numeric_artifacts[4].value,
        result.output
    );

    std::filesystem::remove_all(directory);
}

/**
 * @file m2_2_pipeline_test.cpp
 * @brief Tests the integrated headless flow of Laboratory M2.2.
 */

#include "pdi/io/component_features_csv_storage.hpp"
#include "pdi/io/processing_data_storage.hpp"
#include "pdi/segmentation/component_feature_extractor.hpp"
#include "pdi/segmentation/connected_component_labeler.hpp"
#include "pdi/segmentation/label_visualizer.hpp"
#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <cstdint>
#include <filesystem>
#include <string>

TEST_CASE(
    "Laboratory M2.2 headless flow writes reproducible artifacts",
    "[integration][m2.2]"
) {
    const auto directory =
        std::filesystem::temp_directory_path() / "pdi_labs_m2_2";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    const cv::Mat binary = (
        cv::Mat_<std::uint8_t>(4, 5)
        << 255, 255, 0, 0, 0,
           255, 0, 0, 255, 255,
           0, 0, 0, 255, 0,
           0, 0, 0, 0, 255
    );

    const auto labeling =
        pdi::segmentation::ConnectedComponentLabeler{}.label(
            binary,
            pdi::segmentation::Connectivity::Eight
        );

    const pdi::segmentation::ComponentFeatureExtractor extractor;
    const auto features = extractor.extract(labeling.labels);
    const cv::Mat labels_image =
        pdi::segmentation::LabelVisualizer{}.colorize(labeling.labels);

    const auto labels_path = directory / "labels.png";
    const auto csv_path = directory / "components.csv";
    const auto yaml_path = directory / "components_result.yml";

    REQUIRE(cv::imwrite(labels_path.string(), labels_image));
    pdi::io::ComponentFeaturesCsvStorage{}.save(csv_path, features);

    auto record = extractor.make_processing_record(
        labeling.labels,
        features,
        {
            .project_version = "0.6.0",
            .laboratory = "M2.2",
            .operation = "connected_components",
            .input_path = "synthetic",
            .connectivity = "8",
        }
    );

    record.parameters.push_back({"show", "false"});
    record.parameters.push_back({"interactive", "false"});
    record.parameters.push_back({
        "metric.component_count",
        std::to_string(features.size()),
    });

    pdi::io::ProcessingDataStorage{}.save_yaml(yaml_path, record);

    REQUIRE(std::filesystem::exists(labels_path));
    REQUIRE(std::filesystem::file_size(labels_path) > 0);
    REQUIRE(std::filesystem::exists(csv_path));
    REQUIRE(std::filesystem::file_size(csv_path) > 0);
    REQUIRE(std::filesystem::exists(yaml_path));
    REQUIRE(std::filesystem::file_size(yaml_path) > 0);

    const auto loaded =
        pdi::io::ProcessingDataStorage{}.load_yaml(yaml_path);

    REQUIRE(loaded.project_version == "0.6.0");
    REQUIRE(loaded.laboratory == "M2.2");
    REQUIRE(loaded.numeric_artifacts.size() == 1);
    REQUIRE(loaded.numeric_artifacts[0].name == "labels");

    pdi::testing::require_mat_exact(
        loaded.numeric_artifacts[0].value,
        labeling.labels
    );

    std::filesystem::remove_all(directory);
}

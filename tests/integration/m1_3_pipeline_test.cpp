/**
 * @file m1_3_pipeline_test.cpp
 * @brief Tests the integrated Laboratory M1.3 pipeline and persistence.
 */

#include "pdi/io/image_file_storage.hpp"
#include "pdi/io/processing_data_storage.hpp"
#include "pdi/labs/m1_3_pipeline.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cstdint>
#include <filesystem>

TEST_CASE(
    "M13Pipeline produces identified smoothing output",
    "[integration][m1-3]"
) {
    cv::Mat input(7, 7, CV_8UC1, cv::Scalar{80});
    input.ptr<std::uint8_t>(3)[3] = 255;

    const auto result = pdi::labs::M13Pipeline{}.run(
        input,
        {
            .operation = pdi::labs::M13Operation::Mean3x3,
            .border_strategy =
                pdi::spatial::BorderStrategy::ReplicateBorder,
            .enhancement_factor = 1.0,
        }
    );

    REQUIRE(result.visual_outputs.size() == 1);
    REQUIRE(result.visual_outputs[0].name == "mean_3x3");
    REQUIRE(result.visual_outputs[0].image.type() == CV_8UC1);
    REQUIRE(
        result.visual_outputs[0].image.ptr<std::uint8_t>(3)[3]
        == 99
    );
    REQUIRE(result.numeric_artifacts.size() == 1);
    REQUIRE(result.numeric_artifacts[0].name == "kernel");
}

TEST_CASE(
    "M13Pipeline produces complete Sobel output set",
    "[integration][m1-3]"
) {
    const cv::Mat input = (
        cv::Mat_<std::uint8_t>(3, 3)
            << 0, 10, 20,
               0, 10, 20,
               0, 10, 20
    );

    const auto result = pdi::labs::M13Pipeline{}.run(
        input,
        {
            .operation = pdi::labs::M13Operation::Sobel,
            .border_strategy =
                pdi::spatial::BorderStrategy::CopyBorder,
            .enhancement_factor = 1.0,
        }
    );

    REQUIRE(result.visual_outputs.size() == 4);
    REQUIRE(result.visual_outputs[0].name == "sobel_gx");
    REQUIRE(result.visual_outputs[1].name == "sobel_gy");
    REQUIRE(
        result.visual_outputs[2].name
        == "sobel_magnitude_approximate"
    );
    REQUIRE(
        result.visual_outputs[3].name
        == "sobel_magnitude_euclidean"
    );
    REQUIRE(result.numeric_artifacts.size() == 6);
    REQUIRE(result.numeric_artifacts[2].name == "gradient_x");
    REQUIRE(result.numeric_artifacts[3].name == "gradient_y");
}

TEST_CASE(
    "M13Pipeline outputs can be persisted with pdi io",
    "[integration][m1-3][io]"
) {
    const auto directory =
        std::filesystem::temp_directory_path()
        / "pdi_labs_m1_3_integration";

    std::filesystem::remove_all(directory);

    const cv::Mat input(5, 5, CV_8UC1, cv::Scalar{100});
    const auto result = pdi::labs::M13Pipeline{}.run(
        input,
        {
            .operation = pdi::labs::M13Operation::Laplacian4,
            .border_strategy =
                pdi::spatial::BorderStrategy::ReplicateBorder,
            .enhancement_factor = 0.5,
        }
    );

    const pdi::io::ImageFileStorage storage;

    for (const auto& output : result.visual_outputs) {
        storage.save(directory / (output.name + ".png"), output.image);
        REQUIRE(
            std::filesystem::exists(
                directory / (output.name + ".png")
            )
        );
    }

    const auto data_path = directory / result.data_file_name;
    pdi::io::ProcessingDataStorage{}.save_yaml(
        data_path,
        {
            .format_version = "1",
            .project_version = "0.4.0",
            .laboratory = "M1.3",
            .operation = result.operation_name,
            .input_path = "synthetic",
            .parameters = result.parameters,
            .numeric_artifacts = result.numeric_artifacts,
        }
    );

    const auto loaded =
        pdi::io::ProcessingDataStorage{}.load_yaml(data_path);

    REQUIRE(loaded.operation == "laplacian_4");
    REQUIRE(loaded.numeric_artifacts.size() == 3);
    REQUIRE(loaded.numeric_artifacts[1].name == "raw_response");
    REQUIRE(loaded.numeric_artifacts[1].value.type() == CV_64FC1);
    REQUIRE(
        loaded.numeric_artifacts[1].value.ptr<double>(0)[1] == 0.0
    );

    std::filesystem::remove_all(directory);
}

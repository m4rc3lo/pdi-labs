/**
 * @file processing_data_storage_test.cpp
 * @brief Tests YAML round-trip for generic processing records.
 */

#include "pdi/io/processing_data_storage.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <filesystem>
#include <fstream>
#include <string>

namespace {

[[nodiscard]] std::filesystem::path temporary_directory() {
    return std::filesystem::temp_directory_path()
        / "pdi_labs_processing_data_storage";
}

} // namespace

TEST_CASE(
    "ProcessingDataStorage round-trips metadata without matrices",
    "[unit][io][yaml]"
) {
    const auto directory = temporary_directory();
    const auto file_path = directory / "parameters_only.yml";
    std::filesystem::remove_all(directory);

    const pdi::io::ProcessingRecord expected{
        .format_version = "1",
        .project_version = "0.4.0",
        .laboratory = "M1.1",
        .operation = "quantization",
        .input_path = "images/input/example.pgm",
        .parameters = {{"levels", "8"}},
        .numeric_artifacts = {},
    };

    const pdi::io::ProcessingDataStorage storage;
    storage.save_yaml(file_path, expected);
    const auto actual = storage.load_yaml(file_path);

    REQUIRE(actual.format_version == expected.format_version);
    REQUIRE(actual.project_version == expected.project_version);
    REQUIRE(actual.laboratory == expected.laboratory);
    REQUIRE(actual.operation == expected.operation);
    REQUIRE(actual.input_path == expected.input_path);
    REQUIRE(actual.parameters.size() == 1);
    REQUIRE(actual.parameters[0].name == "levels");
    REQUIRE(actual.parameters[0].value == "8");
    REQUIRE(actual.numeric_artifacts.empty());

    std::filesystem::remove_all(directory);
}

TEST_CASE(
    "ProcessingDataStorage preserves supported matrix types and values",
    "[unit][io][yaml]"
) {
    const auto directory = temporary_directory();
    const auto file_path = directory / "matrices.yaml";
    std::filesystem::remove_all(directory);

    const cv::Mat bytes = (
        cv::Mat_<unsigned char>(1, 3) << 0, 128, 255
    );
    const cv::Mat floats = (
        cv::Mat_<float>(1, 3) << -100.5F, 0.0F, 400.25F
    );
    const cv::Mat doubles = (
        cv::Mat_<double>(1, 3) << -100.5, 0.0, 400.25
    );

    const pdi::io::ProcessingRecord expected{
        .format_version = "1",
        .project_version = "0.4.0",
        .laboratory = "M1.3",
        .operation = "numeric_round_trip",
        .input_path = "synthetic",
        .parameters = {},
        .numeric_artifacts = {
            {"bytes", bytes},
            {"floats", floats},
            {"doubles", doubles},
        },
    };

    const pdi::io::ProcessingDataStorage storage;
    storage.save_yaml(file_path, expected);
    const auto actual = storage.load_yaml(file_path);

    REQUIRE(actual.numeric_artifacts.size() == 3);
    pdi::testing::require_mat_exact(
        actual.numeric_artifacts[0].value,
        bytes
    );
    pdi::testing::require_mat_near(
        actual.numeric_artifacts[1].value,
        floats,
        1.0e-6
    );
    pdi::testing::require_mat_near(
        actual.numeric_artifacts[2].value,
        doubles,
        1.0e-12
    );

    std::filesystem::remove_all(directory);
}

TEST_CASE(
    "ProcessingDataStorage rejects missing and malformed YAML",
    "[unit][io][yaml]"
) {
    const auto directory = temporary_directory();
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    const pdi::io::ProcessingDataStorage storage;

    REQUIRE_THROWS_WITH(
        storage.load_yaml(directory / "missing.yml"),
        Catch::Matchers::ContainsSubstring(
            "Could not open processing data file for reading"
        )
    );

    const auto invalid_path = directory / "invalid.yml";
    std::ofstream invalid_file(invalid_path);
    invalid_file << "%YAML:1.0\noperation: sobel\n";
    invalid_file.close();

    REQUIRE_THROWS_WITH(
        storage.load_yaml(invalid_path),
        Catch::Matchers::ContainsSubstring("format_version")
    );

    std::filesystem::remove_all(directory);
}

TEST_CASE(
    "ProcessingDataStorage rejects empty artifact matrices",
    "[unit][io][yaml]"
) {
    const pdi::io::ProcessingRecord record{
        .format_version = "1",
        .project_version = "0.4.0",
        .laboratory = "M1.3",
        .operation = "invalid",
        .input_path = "synthetic",
        .parameters = {},
        .numeric_artifacts = {{"empty", cv::Mat{}}},
    };

    REQUIRE_THROWS_WITH(
        pdi::io::ProcessingDataStorage{}.save_yaml(
            temporary_directory() / "empty.yml",
            record
        ),
        Catch::Matchers::ContainsSubstring(
            "Numeric artifact matrix must not be empty"
        )
    );
}

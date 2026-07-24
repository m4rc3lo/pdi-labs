/**
 * @file m1_1_pipeline_test.cpp
 * @brief Tests the complete Laboratory M1.1 integration pipeline.
 */

#include "pdi/labs/m1_1_pipeline.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

[[nodiscard]] std::filesystem::path unique_test_directory() {
    const auto suffix =
        std::chrono::steady_clock::now().time_since_epoch().count();

    return std::filesystem::temp_directory_path()
        / ("pdi_labs_m1_1_integration_" + std::to_string(suffix));
}

} // namespace

TEST_CASE(
    "M11Pipeline generates the complete output set",
    "[integration][m1-1]"
) {
    const std::filesystem::path test_directory = unique_test_directory();
    const std::filesystem::path input_path =
        test_directory / "input.png";
    const std::filesystem::path output_directory =
        test_directory / "output";

    REQUIRE(std::filesystem::create_directories(test_directory));

    cv::Mat input(2, 2, CV_8UC3);
    auto* row0 = input.ptr<cv::Vec3b>(0);
    row0[0] = cv::Vec3b{10, 20, 30};
    row0[1] = cv::Vec3b{40, 50, 60};

    auto* row1 = input.ptr<cv::Vec3b>(1);
    row1[0] = cv::Vec3b{70, 80, 90};
    row1[1] = cv::Vec3b{100, 110, 120};

    REQUIRE(cv::imwrite(input_path.string(), input));

    const pdi::labs::M11Results results =
        pdi::labs::M11Pipeline{}.run(input_path, output_directory);

    REQUIRE(results.input.type() == CV_8UC3);
    REQUIRE(results.manual_copy.type() == CV_8UC3);
    REQUIRE(results.blue_channel.type() == CV_8UC1);
    REQUIRE(results.grayscale_average.type() == CV_8UC1);
    REQUIRE(results.quantized_2.type() == CV_8UC1);

    const std::vector<std::string> expected_files{
        "inspection_report.csv",
        "manual_copy.png",
        "channel_blue.png",
        "channel_green.png",
        "channel_red.png",
        "grayscale_average.png",
        "grayscale_weighted.png",
        "quantized_016_levels.png",
        "quantized_008_levels.png",
        "quantized_004_levels.png",
        "quantized_002_levels.png",
    };

    for (const std::string& filename : expected_files) {
        REQUIRE(
            std::filesystem::is_regular_file(output_directory / filename)
        );
    }

    {
        std::ifstream report(output_directory / "inspection_report.csv");
        REQUIRE(report.is_open());

        const std::string report_text{
            std::istreambuf_iterator<char>{report},
            std::istreambuf_iterator<char>{},
        };

        REQUIRE(report_text.find("width,2") != std::string::npos);
        REQUIRE(report_text.find("height,2") != std::string::npos);
        REQUIRE(report_text.find("channel_count,3") != std::string::npos);
        REQUIRE(report_text.find("scope,minimum,maximum,sum,mean")
            != std::string::npos);
    }

    std::filesystem::remove_all(test_directory);
}

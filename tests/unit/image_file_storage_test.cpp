/**
 * @file image_file_storage_test.cpp
 * @brief Tests shared image persistence without opening graphical windows.
 */

#include "pdi/io/image_file_storage.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>

namespace {

[[nodiscard]] std::filesystem::path unique_test_directory() {
    const auto suffix =
        std::chrono::steady_clock::now().time_since_epoch().count();

    return std::filesystem::temp_directory_path()
        / ("pdi_labs_image_storage_" + std::to_string(suffix));
}

} // namespace

TEST_CASE(
    "ImageFileStorage saves and loads a color image",
    "[unit][io][storage]"
) {
    const std::filesystem::path test_directory = unique_test_directory();
    const std::filesystem::path output_path =
        test_directory / "nested" / "sample.png";

    cv::Mat expected(2, 2, CV_8UC3);

    auto* row0 = expected.ptr<cv::Vec3b>(0);
    row0[0] = cv::Vec3b{10, 20, 30};
    row0[1] = cv::Vec3b{40, 50, 60};

    auto* row1 = expected.ptr<cv::Vec3b>(1);
    row1[0] = cv::Vec3b{70, 80, 90};
    row1[1] = cv::Vec3b{100, 110, 120};

    const pdi::io::ImageFileStorage storage;
    storage.save(output_path, expected);

    REQUIRE(std::filesystem::is_regular_file(output_path));

    const cv::Mat loaded = storage.load_color(output_path);
    pdi::testing::require_mat_exact(loaded, expected);

    std::filesystem::remove_all(test_directory);
}

TEST_CASE(
    "ImageFileStorage reports the missing input path",
    "[unit][io][storage]"
) {
    const std::filesystem::path missing_path =
        unique_test_directory() / "missing.png";

    REQUIRE_THROWS_WITH(
        pdi::io::ImageFileStorage{}.load_color(missing_path),
        "Could not read color image: " + missing_path.string()
    );
}

TEST_CASE(
    "ImageFileStorage reports the output path for an empty image",
    "[unit][io][storage]"
) {
    const std::filesystem::path output_path =
        unique_test_directory() / "empty.png";

    REQUIRE_THROWS_WITH(
        pdi::io::ImageFileStorage{}.save(output_path, cv::Mat{}),
        "Could not write empty image: " + output_path.string()
    );
}

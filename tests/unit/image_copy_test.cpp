/**
 * @file image_copy_test.cpp
 * @brief Tests manual deep copying of grayscale and BGR images.
 */

#include "pdi/value/image_copy.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE("ImageCopy creates an independent grayscale copy", "[unit][m1-1][copy]") {
    cv::Mat input_image(2, 3, CV_8UC1);

    auto* first_row = input_image.ptr<std::uint8_t>(0);
    first_row[0] = 10;
    first_row[1] = 20;
    first_row[2] = 30;

    auto* second_row = input_image.ptr<std::uint8_t>(1);
    second_row[0] = 40;
    second_row[1] = 50;
    second_row[2] = 60;

    const cv::Mat original_snapshot = pdi::value::ImageCopy{}.copy(input_image);
    cv::Mat output_image = pdi::value::ImageCopy{}.copy(input_image);

    pdi::testing::require_mat_exact(output_image, input_image);
    REQUIRE(output_image.data != input_image.data);

    output_image.ptr<std::uint8_t>(0)[0] = 255;

    REQUIRE(input_image.ptr<std::uint8_t>(0)[0] == std::uint8_t{10});
    pdi::testing::require_mat_exact(input_image, original_snapshot);

    input_image.ptr<std::uint8_t>(1)[2] = 0;

    REQUIRE(output_image.ptr<std::uint8_t>(1)[2] == std::uint8_t{60});
}

TEST_CASE("ImageCopy creates an independent BGR copy", "[unit][m1-1][copy]") {
    cv::Mat input_image(2, 2, CV_8UC3);

    auto* first_row = input_image.ptr<cv::Vec3b>(0);
    first_row[0] = cv::Vec3b{10, 20, 30};
    first_row[1] = cv::Vec3b{40, 50, 60};

    auto* second_row = input_image.ptr<cv::Vec3b>(1);
    second_row[0] = cv::Vec3b{70, 80, 90};
    second_row[1] = cv::Vec3b{100, 110, 120};

    const cv::Mat original_snapshot = pdi::value::ImageCopy{}.copy(input_image);
    cv::Mat output_image = pdi::value::ImageCopy{}.copy(input_image);

    pdi::testing::require_mat_exact(output_image, input_image);
    REQUIRE(output_image.data != input_image.data);

    output_image.ptr<cv::Vec3b>(0)[0] = cv::Vec3b{200, 201, 202};

    const cv::Vec3b expected_input_pixel{10, 20, 30};
    REQUIRE(input_image.ptr<cv::Vec3b>(0)[0] == expected_input_pixel);
    pdi::testing::require_mat_exact(input_image, original_snapshot);

    input_image.ptr<cv::Vec3b>(1)[1] = cv::Vec3b{0, 0, 0};

    const cv::Vec3b expected_output_pixel{100, 110, 120};
    REQUIRE(output_image.ptr<cv::Vec3b>(1)[1] == expected_output_pixel);
}

TEST_CASE("ImageCopy rejects unsupported images", "[unit][m1-1][copy]") {
    const cv::Mat empty_image;
    const cv::Mat wrong_depth(1, 1, CV_16UC1, cv::Scalar{0});
    const cv::Mat wrong_channels(1, 1, CV_8UC4, cv::Scalar{0, 0, 0, 0});

    const pdi::value::ImageCopy copier;

    REQUIRE_THROWS_WITH(
        copier.copy(empty_image),
        "Image validation failed: the image is empty."
    );

    REQUIRE_THROWS_WITH(
        copier.copy(wrong_depth),
        "Image validation failed: expected CV_8U depth, but received depth code 2."
    );

    REQUIRE_THROWS_WITH(
        copier.copy(wrong_channels),
        "Image copy failed: expected CV_8UC1 or CV_8UC3, but received 4 channel(s)."
    );
}

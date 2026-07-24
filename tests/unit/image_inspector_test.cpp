/**
 * @file image_inspector_test.cpp
 * @brief Tests manual image inspection with small synthetic images.
 */

#include "pdi/value/image_inspector.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE("ImageInspector calculates grayscale statistics", "[unit][m1-1][inspection]") {
    cv::Mat image(2, 3, CV_8UC1);

    auto* first_row = image.ptr<std::uint8_t>(0);
    first_row[0] = 10;
    first_row[1] = 20;
    first_row[2] = 30;

    auto* second_row = image.ptr<std::uint8_t>(1);
    second_row[0] = 40;
    second_row[1] = 50;
    second_row[2] = 60;

    const pdi::value::ImageStatistics result =
        pdi::value::ImageInspector{}.inspect(image);

    REQUIRE(result.width == 3);
    REQUIRE(result.height == 2);
    REQUIRE(result.pixel_count == 6);
    REQUIRE(result.channel_count == 1);
    REQUIRE(result.opencv_type == CV_8UC1);
    REQUIRE(result.intensity.minimum == std::uint8_t{10});
    REQUIRE(result.intensity.maximum == std::uint8_t{60});
    REQUIRE(result.intensity.sum == 210);
    REQUIRE(result.intensity.mean == Catch::Approx(35.0));
    REQUIRE_FALSE(result.bgr.has_value());
}

TEST_CASE("ImageInspector calculates global and BGR statistics", "[unit][m1-1][inspection]") {
    cv::Mat image(2, 2, CV_8UC3);

    auto* first_row = image.ptr<cv::Vec3b>(0);
    first_row[0] = cv::Vec3b{10, 20, 30};
    first_row[1] = cv::Vec3b{40, 50, 60};

    auto* second_row = image.ptr<cv::Vec3b>(1);
    second_row[0] = cv::Vec3b{70, 80, 90};
    second_row[1] = cv::Vec3b{100, 110, 120};

    const pdi::value::ImageStatistics result =
        pdi::value::ImageInspector{}.inspect(image);

    REQUIRE(result.width == 2);
    REQUIRE(result.height == 2);
    REQUIRE(result.pixel_count == 4);
    REQUIRE(result.channel_count == 3);
    REQUIRE(result.opencv_type == CV_8UC3);

    REQUIRE(result.intensity.minimum == std::uint8_t{10});
    REQUIRE(result.intensity.maximum == std::uint8_t{120});
    REQUIRE(result.intensity.sum == 780);
    REQUIRE(result.intensity.mean == Catch::Approx(65.0));

    REQUIRE(result.bgr.has_value());
    const pdi::value::BgrStatistics& bgr = result.bgr.value();

    REQUIRE(bgr.blue.minimum == std::uint8_t{10});
    REQUIRE(bgr.blue.maximum == std::uint8_t{100});
    REQUIRE(bgr.blue.sum == 220);
    REQUIRE(bgr.blue.mean == Catch::Approx(55.0));

    REQUIRE(bgr.green.minimum == std::uint8_t{20});
    REQUIRE(bgr.green.maximum == std::uint8_t{110});
    REQUIRE(bgr.green.sum == 260);
    REQUIRE(bgr.green.mean == Catch::Approx(65.0));

    REQUIRE(bgr.red.minimum == std::uint8_t{30});
    REQUIRE(bgr.red.maximum == std::uint8_t{120});
    REQUIRE(bgr.red.sum == 300);
    REQUIRE(bgr.red.mean == Catch::Approx(75.0));
}

TEST_CASE("ImageInspector rejects unsupported images", "[unit][m1-1][inspection]") {
    const cv::Mat empty_image;
    const cv::Mat wrong_depth(1, 1, CV_16UC1, cv::Scalar{0});
    const cv::Mat wrong_channels(1, 1, CV_8UC4, cv::Scalar{0, 0, 0, 0});

    const pdi::value::ImageInspector inspector;

    REQUIRE_THROWS_WITH(
        inspector.inspect(empty_image),
        "Image validation failed: the image is empty."
    );

    REQUIRE_THROWS_WITH(
        inspector.inspect(wrong_depth),
        "Image validation failed: expected CV_8U depth, but received depth code 2."
    );

    REQUIRE_THROWS_WITH(
        inspector.inspect(wrong_channels),
        "Image inspection failed: expected CV_8UC1 or CV_8UC3, but received 4 channel(s)."
    );
}

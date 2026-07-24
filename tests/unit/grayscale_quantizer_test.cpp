/**
 * @file grayscale_quantizer_test.cpp
 * @brief Tests manual grayscale quantization and boundary behavior.
 */

#include "pdi/value/grayscale_quantizer.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <opencv2/core.hpp>

#include <cstdint>

TEST_CASE(
    "GrayscaleQuantizer maps two-level boundaries exactly",
    "[unit][m1-1][quantization]"
) {
    cv::Mat input(1, 6, CV_8UC1);
    auto* input_row = input.ptr<std::uint8_t>(0);
    input_row[0] = 0;
    input_row[1] = 1;
    input_row[2] = 127;
    input_row[3] = 128;
    input_row[4] = 254;
    input_row[5] = 255;

    cv::Mat expected(1, 6, CV_8UC1);
    auto* expected_row = expected.ptr<std::uint8_t>(0);
    expected_row[0] = 0;
    expected_row[1] = 0;
    expected_row[2] = 0;
    expected_row[3] = 255;
    expected_row[4] = 255;
    expected_row[5] = 255;

    const cv::Mat result =
        pdi::value::GrayscaleQuantizer{}.quantize(input, 2);

    pdi::testing::require_mat_exact(result, expected);
}

TEST_CASE(
    "GrayscaleQuantizer reconstructs four endpoint-preserving levels",
    "[unit][m1-1][quantization]"
) {
    cv::Mat input(1, 8, CV_8UC1);
    auto* input_row = input.ptr<std::uint8_t>(0);
    input_row[0] = 0;
    input_row[1] = 63;
    input_row[2] = 64;
    input_row[3] = 127;
    input_row[4] = 128;
    input_row[5] = 191;
    input_row[6] = 192;
    input_row[7] = 255;

    cv::Mat expected(1, 8, CV_8UC1);
    auto* expected_row = expected.ptr<std::uint8_t>(0);
    expected_row[0] = 0;
    expected_row[1] = 0;
    expected_row[2] = 85;
    expected_row[3] = 85;
    expected_row[4] = 170;
    expected_row[5] = 170;
    expected_row[6] = 255;
    expected_row[7] = 255;

    const cv::Mat result =
        pdi::value::GrayscaleQuantizer{}.quantize(input, 4);

    pdi::testing::require_mat_exact(result, expected);
}

TEST_CASE(
    "GrayscaleQuantizer preserves every value at 256 levels",
    "[unit][m1-1][quantization]"
) {
    cv::Mat input(1, 5, CV_8UC1);
    auto* input_row = input.ptr<std::uint8_t>(0);
    input_row[0] = 0;
    input_row[1] = 1;
    input_row[2] = 127;
    input_row[3] = 128;
    input_row[4] = 255;

    const cv::Mat result =
        pdi::value::GrayscaleQuantizer{}.quantize(input, 256);

    pdi::testing::require_mat_exact(result, input);
    REQUIRE(result.data != input.data);
}

TEST_CASE(
    "GrayscaleQuantizer validates level counts",
    "[unit][m1-1][quantization]"
) {
    const cv::Mat input(1, 1, CV_8UC1, cv::Scalar{0});
    const pdi::value::GrayscaleQuantizer quantizer;

    REQUIRE_THROWS_WITH(
        quantizer.quantize(input, 3),
        "Grayscale quantization failed: levels must be a power of two "
        "in [2, 256], but received 3."
    );

    REQUIRE_THROWS_WITH(
        quantizer.quantize(input, 32),
        "Grayscale quantization failed: supported levels are "
        "2, 4, 8, 16, and 256, but received 32."
    );

    REQUIRE_THROWS_WITH(
        quantizer.quantize(input, 512),
        "Grayscale quantization failed: levels must be a power of two "
        "in [2, 256], but received 512."
    );
}

TEST_CASE(
    "GrayscaleQuantizer rejects non-grayscale input",
    "[unit][m1-1][quantization]"
) {
    const cv::Mat color(1, 1, CV_8UC3, cv::Scalar{0, 0, 0});

    REQUIRE_THROWS_WITH(
        pdi::value::GrayscaleQuantizer{}.quantize(color, 4),
        "Image validation failed: expected 1 channel(s), but received 3."
    );
}

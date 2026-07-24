/**
 * @file grayscale_quantizer.cpp
 * @brief Implements manual grayscale quantization.
 */

#include "pdi/value/grayscale_quantizer.hpp"

#include "pdi/core/image_validator.hpp"

#include <opencv2/core.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace {

[[nodiscard]] bool is_power_of_two(int value) {
    return value > 0 && (value & (value - 1)) == 0;
}

[[nodiscard]] bool is_supported_level_count(int levels) {
    return levels == 2
        || levels == 4
        || levels == 8
        || levels == 16
        || levels == 256;
}

void validate_levels(int levels) {
    if (levels < 2 || levels > 256 || !is_power_of_two(levels)) {
        throw std::invalid_argument(
            "Grayscale quantization failed: levels must be a power of two "
            "in [2, 256], but received "
            + std::to_string(levels)
            + "."
        );
    }

    if (!is_supported_level_count(levels)) {
        throw std::invalid_argument(
            "Grayscale quantization failed: supported levels are "
            "2, 4, 8, 16, and 256, but received "
            + std::to_string(levels)
            + "."
        );
    }
}

[[nodiscard]] std::uint8_t reconstruct_level(int bin_index, int levels) {
    const double reconstructed =
        static_cast<double>(bin_index) * 255.0
        / static_cast<double>(levels - 1);

    return static_cast<std::uint8_t>(std::lround(reconstructed));
}

} // namespace

namespace pdi::value {

cv::Mat GrayscaleQuantizer::quantize(
    const cv::Mat& input_image,
    int levels
) const {
    core::ImageValidator::require_not_empty(input_image);
    core::ImageValidator::require_depth_8u(input_image);
    core::ImageValidator::require_channels(input_image, 1);
    validate_levels(levels);

    cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<std::uint8_t>(row);
        auto* output_row = output_image.ptr<std::uint8_t>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            const int intensity = static_cast<int>(input_row[col]);
            const int bin_index = std::min(
                (intensity * levels) / 256,
                levels - 1
            );

            output_row[col] = reconstruct_level(bin_index, levels);
        }
    }

    return output_image;
}

} // namespace pdi::value

/**
 * @file manual_threshold.cpp
 * @brief Implements manual thresholding for grayscale images.
 */

#include "pdi/segmentation/manual_threshold.hpp"

#include "pdi/core/image_validator.hpp"

#include <opencv2/core.hpp>

#include <cstdint>
#include <stdexcept>

namespace {

void validate_grayscale_image(const cv::Mat& input_image) {
    pdi::core::ImageValidator::require_not_empty(input_image);
    pdi::core::ImageValidator::require_depth_8u(input_image);
    pdi::core::ImageValidator::require_channels(input_image, 1);
}

} // namespace

namespace pdi::segmentation {

cv::Mat ManualThreshold::binary_global(
    const cv::Mat& input_image,
    std::uint8_t threshold
) const {
    validate_grayscale_image(input_image);

    cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<std::uint8_t>(row);
        auto* output_row = output_image.ptr<std::uint8_t>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            output_row[col] =
                input_row[col] >= threshold
                    ? static_cast<std::uint8_t>(255)
                    : static_cast<std::uint8_t>(0);
        }
    }

    return output_image;
}

cv::Mat ManualThreshold::select_interval(
    const cv::Mat& input_image,
    std::uint8_t minimum_value,
    std::uint8_t maximum_value
) const {
    validate_grayscale_image(input_image);

    if (minimum_value > maximum_value) {
        throw std::invalid_argument(
            "Manual interval threshold failed: minimum value must not "
            "exceed maximum value."
        );
    }

    cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<std::uint8_t>(row);
        auto* output_row = output_image.ptr<std::uint8_t>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            const std::uint8_t value = input_row[col];
            output_row[col] =
                value >= minimum_value && value <= maximum_value
                    ? static_cast<std::uint8_t>(255)
                    : static_cast<std::uint8_t>(0);
        }
    }

    return output_image;
}

} // namespace pdi::segmentation

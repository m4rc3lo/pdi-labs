/**
 * @file image_validator.cpp
 * @brief Implements common validation operations for OpenCV images.
 */

#include "pdi/core/image_validator.hpp"

#include <opencv2/core.hpp>

#include <stdexcept>
#include <string>

namespace pdi::core {

void ImageValidator::require_not_empty(const cv::Mat& image) {
    if (image.empty()) {
        throw std::invalid_argument("Image validation failed: the image is empty.");
    }
}

void ImageValidator::require_channels(const cv::Mat& image, const int expected_channels) {
    require_not_empty(image);

    if (expected_channels <= 0) {
        throw std::invalid_argument(
            "Image validation failed: expected channel count must be positive."
        );
    }

    if (image.channels() != expected_channels) {
        throw std::invalid_argument(
            "Image validation failed: expected "
            + std::to_string(expected_channels)
            + " channel(s), but received "
            + std::to_string(image.channels())
            + "."
        );
    }
}

void ImageValidator::require_depth_8u(const cv::Mat& image) {
    require_not_empty(image);

    if (image.depth() != CV_8U) {
        throw std::invalid_argument(
            "Image validation failed: expected CV_8U depth, but received depth code "
            + std::to_string(image.depth())
            + "."
        );
    }
}

} // namespace pdi::core

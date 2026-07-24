/**
 * @file image_copy.cpp
 * @brief Implements manual deep copying of supported OpenCV images.
 */

#include "pdi/value/image_copy.hpp"

#include "pdi/core/image_validator.hpp"

#include <opencv2/core.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace pdi::value {

cv::Mat ImageCopy::copy(const cv::Mat& input_image) const {
    core::ImageValidator::require_not_empty(input_image);
    core::ImageValidator::require_depth_8u(input_image);

    const int channel_count = input_image.channels();

    if (channel_count != 1 && channel_count != 3) {
        throw std::invalid_argument(
            "Image copy failed: expected CV_8UC1 or CV_8UC3, but received "
            + std::to_string(channel_count)
            + " channel(s)."
        );
    }

    cv::Mat output_image(input_image.rows, input_image.cols, input_image.type());

    if (channel_count == 1) {
        for (int row = 0; row < input_image.rows; ++row) {
            const auto* input_row = input_image.ptr<std::uint8_t>(row);
            auto* output_row = output_image.ptr<std::uint8_t>(row);

            for (int col = 0; col < input_image.cols; ++col) {
                output_row[col] = input_row[col];
            }
        }

        return output_image;
    }

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<cv::Vec3b>(row);
        auto* output_row = output_image.ptr<cv::Vec3b>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            output_row[col] = input_row[col];
        }
    }

    return output_image;
}

} // namespace pdi::value

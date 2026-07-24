/**
 * @file channel_separator.cpp
 * @brief Implements manual BGR channel separation.
 */

#include "pdi/value/channel_separator.hpp"

#include "pdi/core/image_validator.hpp"

#include <opencv2/core.hpp>

#include <cstdint>

namespace pdi::value {

ChannelSet ChannelSeparator::separate(const cv::Mat& input_image) const {
    core::ImageValidator::require_not_empty(input_image);
    core::ImageValidator::require_depth_8u(input_image);
    core::ImageValidator::require_channels(input_image, 3);

    ChannelSet channels{
        .blue = cv::Mat(input_image.rows, input_image.cols, CV_8UC1),
        .green = cv::Mat(input_image.rows, input_image.cols, CV_8UC1),
        .red = cv::Mat(input_image.rows, input_image.cols, CV_8UC1),
    };

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<cv::Vec3b>(row);
        auto* blue_row = channels.blue.ptr<std::uint8_t>(row);
        auto* green_row = channels.green.ptr<std::uint8_t>(row);
        auto* red_row = channels.red.ptr<std::uint8_t>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            const cv::Vec3b pixel = input_row[col];
            blue_row[col] = pixel[0];
            green_row[col] = pixel[1];
            red_row[col] = pixel[2];
        }
    }

    return channels;
}

} // namespace pdi::value

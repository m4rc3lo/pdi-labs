/**
 * @file channel_separator.hpp
 * @brief Declares manual separation of BGR image channels.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::value {

/**
 * @brief Stores the three independent channels extracted from a BGR image.
 *
 * @details Every member is a `CV_8UC1` matrix with the same dimensions as the
 * source image. OpenCV uses BGR order: index 0 is blue, index 1 is green, and
 * index 2 is red.
 */
struct ChannelSet {
    cv::Mat blue;
    cv::Mat green;
    cv::Mat red;
};

/**
 * @brief Separates a three-channel BGR image manually.
 *
 * @details The implementation traverses the source with one `cv::Vec3b*` row
 * pointer and writes directly to three `std::uint8_t*` row pointers. It does
 * not use `cv::split()` or an additional channel loop.
 */
class ChannelSeparator {
public:
    /**
     * @brief Extracts blue, green, and red channels.
     *
     * @param input_image Source image with type `CV_8UC3`.
     * @return Three independent `CV_8UC1` images.
     *
     * @throws std::invalid_argument If the image is empty.
     * @throws std::invalid_argument If its depth is not `CV_8U`.
     * @throws std::invalid_argument If it does not contain exactly three
     * channels.
     *
     * @post The input image remains unchanged.
     */
    [[nodiscard]] ChannelSet separate(const cv::Mat& input_image) const;
};

} // namespace pdi::value

/**
 * @file image_validator.hpp
 * @brief Declares common validation operations for OpenCV images.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::core {

/**
 * @brief Validates structural preconditions of cv::Mat images.
 *
 * @details The class centralizes checks shared by the laboratory algorithms.
 * Validation methods do not modify the supplied image and do not execute image
 * processing operations.
 */
class ImageValidator {
public:
    /**
     * @brief Ensures that an image contains allocated pixel data.
     *
     * @param image Image to validate.
     *
     * @throws std::invalid_argument If the image is empty.
     * @post The image remains unchanged.
     */
    static void require_not_empty(const cv::Mat& image);

    /**
     * @brief Ensures that an image has the expected number of channels.
     *
     * @param image Image to validate.
     * @param expected_channels Required positive channel count.
     *
     * @throws std::invalid_argument If the image is empty.
     * @throws std::invalid_argument If expected_channels is not positive.
     * @throws std::invalid_argument If the channel count differs from the
     * expected value.
     * @post The image remains unchanged.
     */
    static void require_channels(const cv::Mat& image, int expected_channels);

    /**
     * @brief Ensures that an image uses unsigned 8-bit channel values.
     *
     * @param image Image to validate.
     *
     * @throws std::invalid_argument If the image is empty.
     * @throws std::invalid_argument If the image depth is not CV_8U.
     * @post The image remains unchanged.
     */
    static void require_depth_8u(const cv::Mat& image);
};

} // namespace pdi::core

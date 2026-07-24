/**
 * @file grayscale_converter.hpp
 * @brief Declares manual BGR-to-grayscale conversions.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::value {

/**
 * @brief Converts BGR images to grayscale using manual pixel traversal.
 *
 * @details The source image must have type `CV_8UC3`. OpenCV stores channels
 * in BGR order: index 0 is blue, index 1 is green, and index 2 is red.
 *
 * Both methods calculate in `double`, round to the nearest integer with
 * `std::lround`, and saturate the result to `[0, 255]` through the common
 * saturation utility.
 */
class GrayscaleConverter {
public:
    /**
     * @brief Converts a BGR image using the arithmetic mean.
     *
     * @details Computes `(B + G + R) / 3` for each pixel.
     *
     * @param input_image Source image with type `CV_8UC3`.
     * @return Grayscale image with type `CV_8UC1`.
     *
     * @throws std::invalid_argument If the image is empty.
     * @throws std::invalid_argument If its depth is not `CV_8U`.
     * @throws std::invalid_argument If it does not contain exactly three
     * channels.
     */
    [[nodiscard]] cv::Mat convert_average(const cv::Mat& input_image) const;

    /**
     * @brief Converts a BGR image using perceptual luminance weights.
     *
     * @details Computes `0.299 R + 0.587 G + 0.114 B` for each pixel.
     *
     * @param input_image Source image with type `CV_8UC3`.
     * @return Grayscale image with type `CV_8UC1`.
     *
     * @throws std::invalid_argument If the image is empty.
     * @throws std::invalid_argument If its depth is not `CV_8U`.
     * @throws std::invalid_argument If it does not contain exactly three
     * channels.
     */
    [[nodiscard]] cv::Mat convert_weighted(const cv::Mat& input_image) const;
};

} // namespace pdi::value

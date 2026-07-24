/**
 * @file image_copy.hpp
 * @brief Declares manual deep copying of supported OpenCV images.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::value {

/**
 * @brief Creates independent pixel-by-pixel copies of supported images.
 *
 * @details The implementation allocates a new `cv::Mat` with the same
 * dimensions and type as the input image, then copies each pixel through typed
 * row pointers. It does not use `clone()`, `copyTo()`, shallow assignment, or
 * `cv::Mat::at()`.
 */
class ImageCopy {
public:
    /**
     * @brief Copies a grayscale or BGR image manually.
     *
     * @param input_image Input image with type `CV_8UC1` or `CV_8UC3`.
     * @return Independent image with identical dimensions, type, and pixels.
     *
     * @throws std::invalid_argument If the input image is empty.
     * @throws std::invalid_argument If the input depth is not `CV_8U`.
     * @throws std::invalid_argument If the input has neither one nor three
     * channels.
     *
     * @post The input image remains unchanged.
     * @post The returned image does not share its pixel buffer with the input.
     */
    [[nodiscard]] cv::Mat copy(const cv::Mat& input_image) const;
};

} // namespace pdi::value

/**
 * @file manual_threshold.hpp
 * @brief Declares manual thresholding operations for grayscale images.
 */

#pragma once

#include <opencv2/core/mat.hpp>

#include <cstdint>

namespace pdi::segmentation {

/**
 * @brief Applies binary decisions directly to `CV_8UC1` pixels.
 *
 * @details All operations traverse the image manually and produce only the
 * values `0` and `255`. OpenCV is used only for matrix representation.
 */
class ManualThreshold {
public:
    /**
     * @brief Applies one inclusive global threshold.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param threshold Minimum intensity classified as foreground.
     * @return Binary image with type `CV_8UC1`.
     *
     * @details Pixels with value greater than or equal to `threshold` become
     * `255`; lower values become `0`.
     *
     * @throws std::invalid_argument If the image is empty or is not `CV_8UC1`.
     *
     * @par Complexity
     * For an image with `M` rows and `N` columns, time complexity is `O(MN)`
     * and the output requires `O(MN)` additional space.
     */
    [[nodiscard]] cv::Mat binary_global(
        const cv::Mat& input_image,
        std::uint8_t threshold
    ) const;

    /**
     * @brief Selects intensities inside an inclusive interval.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param minimum_value Inclusive lower interval boundary.
     * @param maximum_value Inclusive upper interval boundary.
     * @return Binary image with type `CV_8UC1`.
     *
     * @details Values in `[minimum_value, maximum_value]` become `255`;
     * values outside the interval become `0`.
     *
     * @throws std::invalid_argument If the image is empty or is not `CV_8UC1`.
     * @throws std::invalid_argument If `minimum_value` is greater than
     * `maximum_value`.
     *
     * @par Complexity
     * For an image with `M` rows and `N` columns, time complexity is `O(MN)`
     * and the output requires `O(MN)` additional space.
     */
    [[nodiscard]] cv::Mat select_interval(
        const cv::Mat& input_image,
        std::uint8_t minimum_value,
        std::uint8_t maximum_value
    ) const;
};

} // namespace pdi::segmentation

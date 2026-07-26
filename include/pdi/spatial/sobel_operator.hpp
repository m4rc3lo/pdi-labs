/**
 * @file sobel_operator.hpp
 * @brief Declares the manual Sobel edge operator.
 */

#pragma once

#include "pdi/spatial/spatial_convolution.hpp"

#include <opencv2/core/mat.hpp>

namespace pdi::spatial {

/**
 * @brief Groups signed Sobel gradients, magnitudes and visualizations.
 */
struct SobelResult {
    /** @brief Horizontal derivative response with type `CV_64FC1`. */
    cv::Mat gradient_x;

    /** @brief Vertical derivative response with type `CV_64FC1`. */
    cv::Mat gradient_y;

    /** @brief Approximate magnitude `|Gx| + |Gy|` with type `CV_64FC1`. */
    cv::Mat magnitude_approximate;

    /** @brief Euclidean magnitude `sqrt(Gx^2 + Gy^2)` with type `CV_64FC1`. */
    cv::Mat magnitude_euclidean;

    /** @brief Min-max visualization of signed `Gx` with type `CV_8UC1`. */
    cv::Mat gradient_x_visualization;

    /** @brief Min-max visualization of signed `Gy` with type `CV_8UC1`. */
    cv::Mat gradient_y_visualization;

    /** @brief Min-max visualization of the approximate magnitude. */
    cv::Mat magnitude_approximate_visualization;

    /** @brief Min-max visualization of the Euclidean magnitude. */
    cv::Mat magnitude_euclidean_visualization;
};

/**
 * @brief Computes Sobel gradients manually through SpatialConvolution.
 *
 * @details The implementation preserves signed derivative responses in
 * `CV_64FC1`. No premature conversion to `uchar` is performed, and
 * `cv::Sobel` is not used.
 */
class SobelOperator {
public:
    /**
     * @brief Computes Gx, Gy and two gradient magnitudes.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param border_strategy Border handling strategy.
     * @return Signed gradients, magnitudes and visualizations.
     *
     * @note `Gx` measures intensity variation along the x axis and therefore
     * responds strongly to predominantly vertical edges.
     * @note `Gy` measures intensity variation along the y axis and therefore
     * responds strongly to predominantly horizontal edges.
     */
    [[nodiscard]] SobelResult apply(
        const cv::Mat& input_image,
        BorderStrategy border_strategy = BorderStrategy::ReplicateBorder
    ) const;
};

} // namespace pdi::spatial

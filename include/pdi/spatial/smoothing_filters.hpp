/**
 * @file smoothing_filters.hpp
 * @brief Declares reusable smoothing filters based on SpatialConvolution.
 */

#pragma once

#include "pdi/spatial/spatial_convolution.hpp"

#include <opencv2/core/mat.hpp>

namespace pdi::spatial {

/**
 * @brief Provides didactic linear smoothing filters for grayscale images.
 *
 * @details Every operation delegates pixel processing to
 * `SpatialConvolution::convolution`. Kernels are defined explicitly and
 * normalization is enabled so their coefficient sums preserve constant
 * regions.
 */
class SmoothingFilters {
public:
    /**
     * @brief Applies a uniform 3 x 3 mean filter.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param border_strategy Border handling strategy.
     * @return Smoothed image with the same dimensions and type.
     */
    [[nodiscard]] cv::Mat mean_3x3(
        const cv::Mat& input_image,
        BorderStrategy border_strategy = BorderStrategy::CopyBorder
    ) const;

    /**
     * @brief Applies a center-weighted 3 x 3 mean filter.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param border_strategy Border handling strategy.
     * @return Smoothed image with the same dimensions and type.
     */
    [[nodiscard]] cv::Mat weighted_mean_3x3(
        const cv::Mat& input_image,
        BorderStrategy border_strategy = BorderStrategy::CopyBorder
    ) const;

    /**
     * @brief Applies a uniform 5 x 5 mean filter.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param border_strategy Border handling strategy.
     * @return Smoothed image with the same dimensions and type.
     */
    [[nodiscard]] cv::Mat mean_5x5(
        const cv::Mat& input_image,
        BorderStrategy border_strategy = BorderStrategy::CopyBorder
    ) const;
};

} // namespace pdi::spatial

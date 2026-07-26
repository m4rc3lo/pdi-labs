/**
 * @file laplacian_enhancement.hpp
 * @brief Declares Laplacian response and image enhancement operations.
 */

#pragma once

#include "pdi/spatial/spatial_convolution.hpp"

#include <opencv2/core/mat.hpp>

namespace pdi::spatial {

/**
 * @brief Selects a symmetric 3 x 3 Laplacian kernel.
 */
enum class LaplacianKernel {
    /** @brief Four-neighbor kernel with positive center coefficient. */
    FourNeighbor,

    /** @brief Eight-neighbor kernel with positive center coefficient. */
    EightNeighbor,
};

/**
 * @brief Groups numerical and visual outputs of Laplacian enhancement.
 */
struct LaplacianEnhancementResult {
    /** @brief Signed `CV_64FC1` Laplacian response. */
    cv::Mat raw_response;

    /** @brief Min-max normalized `CV_8UC1` view of the raw response. */
    cv::Mat response_visualization;

    /** @brief Saturated `CV_8UC1` enhanced image. */
    cv::Mat enhanced_image;
};

/**
 * @brief Computes signed Laplacian responses and combines them with the source.
 */
class LaplacianEnhancement {
public:
    /**
     * @brief Applies a selected Laplacian kernel and enhances the input image.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param kernel Selected Laplacian kernel.
     * @param enhancement_factor Multiplier applied to the signed response.
     * @param border_strategy Border handling strategy.
     * @return Raw response, visualization and enhanced image.
     *
     * @details Enhancement uses
     * `enhanced = input + enhancement_factor * raw_response`.
     * The signed response remains in `CV_64FC1` until the final visual output.
     * Only the enhanced image is rounded and saturated to `[0, 255]`.
     */
    [[nodiscard]] LaplacianEnhancementResult apply(
        const cv::Mat& input_image,
        LaplacianKernel kernel,
        double enhancement_factor,
        BorderStrategy border_strategy = BorderStrategy::ReplicateBorder
    ) const;
};

} // namespace pdi::spatial

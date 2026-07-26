/**
 * @file spatial_kernels.hpp
 * @brief Declares reusable kernels used by spatial laboratory operations.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::spatial {

/**
 * @brief Creates canonical kernels shared by processing and traceability.
 *
 * @details Returning matrices from one catalog prevents persisted kernels from
 * drifting away from the coefficients used by the algorithms.
 */
class SpatialKernels {
public:
    [[nodiscard]] static cv::Mat mean_3x3();
    [[nodiscard]] static cv::Mat weighted_mean_3x3();
    [[nodiscard]] static cv::Mat mean_5x5();
    [[nodiscard]] static cv::Mat laplacian_4();
    [[nodiscard]] static cv::Mat laplacian_8();
    [[nodiscard]] static cv::Mat sobel_x();
    [[nodiscard]] static cv::Mat sobel_y();
};

} // namespace pdi::spatial

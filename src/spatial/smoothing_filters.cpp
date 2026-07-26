/**
 * @file smoothing_filters.cpp
 * @brief Implements smoothing filters with the generic convolution core.
 */

#include "pdi/spatial/smoothing_filters.hpp"

#include "pdi/spatial/spatial_kernels.hpp"

namespace pdi::spatial {

cv::Mat SmoothingFilters::mean_3x3(
    const cv::Mat& input_image,
    BorderStrategy border_strategy
) const {
    return SpatialConvolution{}.convolution(
        input_image,
        SpatialKernels::mean_3x3(),
        true,
        border_strategy
    );
}

cv::Mat SmoothingFilters::weighted_mean_3x3(
    const cv::Mat& input_image,
    BorderStrategy border_strategy
) const {
    return SpatialConvolution{}.convolution(
        input_image,
        SpatialKernels::weighted_mean_3x3(),
        true,
        border_strategy
    );
}

cv::Mat SmoothingFilters::mean_5x5(
    const cv::Mat& input_image,
    BorderStrategy border_strategy
) const {
    return SpatialConvolution{}.convolution(
        input_image,
        SpatialKernels::mean_5x5(),
        true,
        border_strategy
    );
}

} // namespace pdi::spatial

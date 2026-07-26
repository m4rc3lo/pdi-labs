/**
 * @file smoothing_filters.cpp
 * @brief Implements smoothing filters with the generic convolution core.
 */

#include "pdi/spatial/smoothing_filters.hpp"

#include <opencv2/core.hpp>

namespace {

[[nodiscard]] cv::Mat mean_kernel_3x3() {
    return (
        cv::Mat_<float>(3, 3)
            << 1.0F, 1.0F, 1.0F,
               1.0F, 1.0F, 1.0F,
               1.0F, 1.0F, 1.0F
    );
}

[[nodiscard]] cv::Mat weighted_mean_kernel_3x3() {
    return (
        cv::Mat_<float>(3, 3)
            << 1.0F, 2.0F, 1.0F,
               2.0F, 4.0F, 2.0F,
               1.0F, 2.0F, 1.0F
    );
}

[[nodiscard]] cv::Mat mean_kernel_5x5() {
    return cv::Mat(5, 5, CV_32FC1, cv::Scalar{1.0F});
}

} // namespace

namespace pdi::spatial {

cv::Mat SmoothingFilters::mean_3x3(
    const cv::Mat& input_image,
    BorderStrategy border_strategy
) const {
    return SpatialConvolution{}.convolution(
        input_image,
        mean_kernel_3x3(),
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
        weighted_mean_kernel_3x3(),
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
        mean_kernel_5x5(),
        true,
        border_strategy
    );
}

} // namespace pdi::spatial

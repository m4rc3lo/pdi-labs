/**
 * @file spatial_kernels.cpp
 * @brief Implements reusable spatial kernels.
 */

#include "pdi/spatial/spatial_kernels.hpp"

#include <opencv2/core.hpp>

namespace pdi::spatial {

cv::Mat SpatialKernels::mean_3x3() {
    return cv::Mat(3, 3, CV_32FC1, cv::Scalar{1.0F});
}

cv::Mat SpatialKernels::weighted_mean_3x3() {
    return (
        cv::Mat_<float>(3, 3)
            << 1.0F, 2.0F, 1.0F,
               2.0F, 4.0F, 2.0F,
               1.0F, 2.0F, 1.0F
    );
}

cv::Mat SpatialKernels::mean_5x5() {
    return cv::Mat(5, 5, CV_32FC1, cv::Scalar{1.0F});
}

cv::Mat SpatialKernels::laplacian_4() {
    return (
        cv::Mat_<double>(3, 3)
            << 0.0, -1.0, 0.0,
               -1.0, 4.0, -1.0,
               0.0, -1.0, 0.0
    );
}

cv::Mat SpatialKernels::laplacian_8() {
    return (
        cv::Mat_<double>(3, 3)
            << -1.0, -1.0, -1.0,
               -1.0, 8.0, -1.0,
               -1.0, -1.0, -1.0
    );
}

cv::Mat SpatialKernels::sobel_x() {
    return (
        cv::Mat_<double>(3, 3)
            << -1.0, 0.0, 1.0,
               -2.0, 0.0, 2.0,
               -1.0, 0.0, 1.0
    );
}

cv::Mat SpatialKernels::sobel_y() {
    return (
        cv::Mat_<double>(3, 3)
            << -1.0, -2.0, -1.0,
                0.0,  0.0,  0.0,
                1.0,  2.0,  1.0
    );
}

} // namespace pdi::spatial

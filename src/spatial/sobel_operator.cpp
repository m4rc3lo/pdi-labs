/**
 * @file sobel_operator.cpp
 * @brief Implements the manual Sobel edge operator.
 */

#include "pdi/spatial/sobel_operator.hpp"

#include "pdi/core/saturation.hpp"
#include "pdi/spatial/spatial_kernels.hpp"

#include <opencv2/core.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace {

[[nodiscard]] cv::Mat normalize_for_visualization(const cv::Mat& input) {
    double minimum = input.ptr<double>(0)[0];
    double maximum = minimum;

    for (int row = 0; row < input.rows; ++row) {
        const auto* input_row = input.ptr<double>(row);

        for (int col = 0; col < input.cols; ++col) {
            minimum = std::min(minimum, input_row[col]);
            maximum = std::max(maximum, input_row[col]);
        }
    }

    cv::Mat visualization = cv::Mat::zeros(
        input.rows,
        input.cols,
        CV_8UC1
    );

    const double range = maximum - minimum;

    if (range <= 1.0e-12) {
        return visualization;
    }

    for (int row = 0; row < input.rows; ++row) {
        const auto* input_row = input.ptr<double>(row);
        auto* output_row = visualization.ptr<std::uint8_t>(row);

        for (int col = 0; col < input.cols; ++col) {
            const double normalized =
                (input_row[col] - minimum) * 255.0 / range;
            output_row[col] =
                pdi::core::saturate_to_byte(normalized);
        }
    }

    return visualization;
}

} // namespace

namespace pdi::spatial {

SobelResult SobelOperator::apply(
    const cv::Mat& input_image,
    BorderStrategy border_strategy
) const {
    const SpatialConvolution convolution;

    const cv::Mat gradient_x = convolution.convolution_raw(
        input_image,
        SpatialKernels::sobel_x(),
        false,
        border_strategy
    );
    const cv::Mat gradient_y = convolution.convolution_raw(
        input_image,
        SpatialKernels::sobel_y(),
        false,
        border_strategy
    );

    cv::Mat magnitude_approximate(
        input_image.rows,
        input_image.cols,
        CV_64FC1
    );
    cv::Mat magnitude_euclidean(
        input_image.rows,
        input_image.cols,
        CV_64FC1
    );

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* gx_row = gradient_x.ptr<double>(row);
        const auto* gy_row = gradient_y.ptr<double>(row);
        auto* approximate_row =
            magnitude_approximate.ptr<double>(row);
        auto* euclidean_row =
            magnitude_euclidean.ptr<double>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            const double gx = gx_row[col];
            const double gy = gy_row[col];

            approximate_row[col] = std::abs(gx) + std::abs(gy);
            euclidean_row[col] = std::sqrt(gx * gx + gy * gy);
        }
    }

    return {
        .gradient_x = gradient_x,
        .gradient_y = gradient_y,
        .magnitude_approximate = magnitude_approximate,
        .magnitude_euclidean = magnitude_euclidean,
        .gradient_x_visualization =
            normalize_for_visualization(gradient_x),
        .gradient_y_visualization =
            normalize_for_visualization(gradient_y),
        .magnitude_approximate_visualization =
            normalize_for_visualization(magnitude_approximate),
        .magnitude_euclidean_visualization =
            normalize_for_visualization(magnitude_euclidean),
    };
}

} // namespace pdi::spatial

/**
 * @file laplacian_enhancement.cpp
 * @brief Implements signed Laplacian response and image enhancement.
 */

#include "pdi/spatial/laplacian_enhancement.hpp"

#include "pdi/core/saturation.hpp"
#include "pdi/spatial/spatial_kernels.hpp"

#include <opencv2/core.hpp>

#include <algorithm>
#include <cstdint>

namespace {

[[nodiscard]] cv::Mat normalize_for_visualization(
    const cv::Mat& raw_response
) {
    double minimum = raw_response.ptr<double>(0)[0];
    double maximum = minimum;

    for (int row = 0; row < raw_response.rows; ++row) {
        const auto* raw_row = raw_response.ptr<double>(row);

        for (int col = 0; col < raw_response.cols; ++col) {
            minimum = std::min(minimum, raw_row[col]);
            maximum = std::max(maximum, raw_row[col]);
        }
    }

    cv::Mat visualization = cv::Mat::zeros(
        raw_response.rows,
        raw_response.cols,
        CV_8UC1
    );

    const double range = maximum - minimum;

    if (range <= 1.0e-12) {
        return visualization;
    }

    for (int row = 0; row < raw_response.rows; ++row) {
        const auto* raw_row = raw_response.ptr<double>(row);
        auto* visual_row = visualization.ptr<std::uint8_t>(row);

        for (int col = 0; col < raw_response.cols; ++col) {
            const double normalized =
                (raw_row[col] - minimum) * 255.0 / range;
            visual_row[col] = pdi::core::saturate_to_byte(normalized);
        }
    }

    return visualization;
}

} // namespace

namespace pdi::spatial {

LaplacianEnhancementResult LaplacianEnhancement::apply(
    const cv::Mat& input_image,
    LaplacianKernel kernel,
    double enhancement_factor,
    BorderStrategy border_strategy
) const {
    const cv::Mat raw_response =
        SpatialConvolution{}.convolution_raw(
            input_image,
            kernel == LaplacianKernel::FourNeighbor
                ? SpatialKernels::laplacian_4()
                : SpatialKernels::laplacian_8(),
            false,
            border_strategy
        );

    cv::Mat enhanced_raw(input_image.rows, input_image.cols, CV_64FC1);
    cv::Mat enhanced_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<std::uint8_t>(row);
        const auto* response_row = raw_response.ptr<double>(row);
        auto* enhanced_raw_row = enhanced_raw.ptr<double>(row);
        auto* output_row = enhanced_image.ptr<std::uint8_t>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            const double enhanced =
                static_cast<double>(input_row[col])
                + enhancement_factor * response_row[col];
            enhanced_raw_row[col] = enhanced;
            output_row[col] = core::saturate_to_byte(enhanced);
        }
    }

    return {
        .raw_response = raw_response,
        .enhanced_raw = enhanced_raw,
        .response_visualization =
            normalize_for_visualization(raw_response),
        .enhanced_image = enhanced_image,
    };
}

} // namespace pdi::spatial

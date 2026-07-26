/**
 * @file spatial_convolution.cpp
 * @brief Implements generic manual spatial correlation named convolution.
 */

#include "pdi/spatial/spatial_convolution.hpp"

#include "pdi/core/image_validator.hpp"
#include "pdi/core/saturation.hpp"

#include <opencv2/core.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace {

void validate_kernel(const cv::Mat& kernel, const cv::Mat& input_image) {
    if (kernel.empty()) {
        throw std::invalid_argument(
            "Spatial convolution failed: kernel must not be empty."
        );
    }

    if (kernel.channels() != 1) {
        throw std::invalid_argument(
            "Spatial convolution failed: kernel must have one channel."
        );
    }

    if (kernel.depth() != CV_32F && kernel.depth() != CV_64F) {
        throw std::invalid_argument(
            "Spatial convolution failed: kernel must use CV_32F or CV_64F."
        );
    }

    if (kernel.rows != kernel.cols) {
        throw std::invalid_argument(
            "Spatial convolution failed: kernel must be square."
        );
    }

    if (kernel.rows % 2 == 0) {
        throw std::invalid_argument(
            "Spatial convolution failed: kernel dimension must be odd."
        );
    }

    if (kernel.rows > input_image.rows || kernel.cols > input_image.cols) {
        throw std::invalid_argument(
            "Spatial convolution failed: kernel must not be larger than "
            "the input image."
        );
    }
}

[[nodiscard]] double kernel_value(
    const cv::Mat& kernel,
    int row,
    int col
) {
    if (kernel.depth() == CV_32F) {
        return static_cast<double>(kernel.ptr<float>(row)[col]);
    }

    return kernel.ptr<double>(row)[col];
}

[[nodiscard]] int clamp_index(int value, int upper_bound) {
    return std::clamp(value, 0, upper_bound - 1);
}

[[nodiscard]] double kernel_sum(const cv::Mat& kernel) {
    double sum = 0.0;

    for (int row = 0; row < kernel.rows; ++row) {
        for (int col = 0; col < kernel.cols; ++col) {
            sum += kernel_value(kernel, row, col);
        }
    }

    return sum;
}

} // namespace

namespace pdi::spatial {

cv::Mat SpatialConvolution::convolution_raw(
    const cv::Mat& input_image,
    const cv::Mat& kernel,
    bool normalize_kernel,
    BorderStrategy border_strategy
) const {
    core::ImageValidator::require_not_empty(input_image);
    core::ImageValidator::require_depth_8u(input_image);
    core::ImageValidator::require_channels(input_image, 1);
    validate_kernel(kernel, input_image);

    double normalization_divisor = 1.0;

    if (normalize_kernel) {
        normalization_divisor = kernel_sum(kernel);

        if (std::abs(normalization_divisor) <= 1.0e-12) {
            throw std::invalid_argument(
                "Spatial convolution failed: normalized kernel sum must not "
                "be zero."
            );
        }
    }

    cv::Mat output_image(input_image.rows, input_image.cols, CV_64FC1);

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<std::uint8_t>(row);
        auto* output_row = output_image.ptr<double>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            output_row[col] = static_cast<double>(input_row[col]);
        }
    }

    const int radius = kernel.rows / 2;
    const int row_begin =
        border_strategy == BorderStrategy::CopyBorder ? radius : 0;
    const int row_end =
        border_strategy == BorderStrategy::CopyBorder
            ? input_image.rows - radius
            : input_image.rows;
    const int col_begin =
        border_strategy == BorderStrategy::CopyBorder ? radius : 0;
    const int col_end =
        border_strategy == BorderStrategy::CopyBorder
            ? input_image.cols - radius
            : input_image.cols;

    for (int row = row_begin; row < row_end; ++row) {
        auto* output_row = output_image.ptr<double>(row);

        for (int col = col_begin; col < col_end; ++col) {
            double accumulated = 0.0;

            for (int kernel_row = 0; kernel_row < kernel.rows; ++kernel_row) {
                int input_row_index = row + kernel_row - radius;

                if (border_strategy == BorderStrategy::ReplicateBorder) {
                    input_row_index = clamp_index(
                        input_row_index,
                        input_image.rows
                    );
                }

                const auto* input_row =
                    input_image.ptr<std::uint8_t>(input_row_index);

                for (int kernel_col = 0;
                     kernel_col < kernel.cols;
                     ++kernel_col) {
                    int input_col_index = col + kernel_col - radius;

                    if (border_strategy == BorderStrategy::ReplicateBorder) {
                        input_col_index = clamp_index(
                            input_col_index,
                            input_image.cols
                        );
                    }

                    accumulated +=
                        static_cast<double>(input_row[input_col_index])
                        * kernel_value(kernel, kernel_row, kernel_col);
                }
            }

            output_row[col] = accumulated / normalization_divisor;
        }
    }

    return output_image;
}

cv::Mat SpatialConvolution::convolution(
    const cv::Mat& input_image,
    const cv::Mat& kernel,
    bool normalize_kernel,
    BorderStrategy border_strategy
) const {
    const cv::Mat raw_response = convolution_raw(
        input_image,
        kernel,
        normalize_kernel,
        border_strategy
    );

    cv::Mat output_image(raw_response.rows, raw_response.cols, CV_8UC1);

    for (int row = 0; row < raw_response.rows; ++row) {
        const auto* raw_row = raw_response.ptr<double>(row);
        auto* output_row = output_image.ptr<std::uint8_t>(row);

        for (int col = 0; col < raw_response.cols; ++col) {
            output_row[col] = core::saturate_to_byte(raw_row[col]);
        }
    }

    return output_image;
}

} // namespace pdi::spatial

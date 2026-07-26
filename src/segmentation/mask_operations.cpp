/**
 * @file mask_operations.cpp
 * @brief Implements manual operations with binary masks.
 */

#include "pdi/segmentation/mask_operations.hpp"

#include "pdi/core/image_validator.hpp"

#include <opencv2/core.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace {

void validate_binary_mask(
    const cv::Mat& mask,
    const std::string& parameter_name
) {
    pdi::core::ImageValidator::require_not_empty(mask);
    pdi::core::ImageValidator::require_depth_8u(mask);
    pdi::core::ImageValidator::require_channels(mask, 1);

    for (int row = 0; row < mask.rows; ++row) {
        const auto* mask_row = mask.ptr<std::uint8_t>(row);

        for (int col = 0; col < mask.cols; ++col) {
            if (mask_row[col] != 0 && mask_row[col] != 255) {
                throw std::invalid_argument(
                    parameter_name
                    + " must contain only binary values 0 and 255."
                );
            }
        }
    }
}

void validate_matching_masks(
    const cv::Mat& first_mask,
    const cv::Mat& second_mask
) {
    validate_binary_mask(first_mask, "First mask");
    validate_binary_mask(second_mask, "Second mask");

    if (first_mask.size() != second_mask.size()) {
        throw std::invalid_argument(
            "Binary masks must have matching dimensions."
        );
    }
}

void validate_image_and_mask(
    const cv::Mat& input_image,
    const cv::Mat& mask,
    int expected_channels
) {
    pdi::core::ImageValidator::require_not_empty(input_image);
    pdi::core::ImageValidator::require_depth_8u(input_image);
    pdi::core::ImageValidator::require_channels(
        input_image,
        expected_channels
    );
    validate_binary_mask(mask, "Mask");

    if (input_image.size() != mask.size()) {
        throw std::invalid_argument(
            "Image and mask must have matching dimensions."
        );
    }
}

} // namespace

namespace pdi::segmentation {

cv::Mat MaskOperations::invert(const cv::Mat& mask) const {
    validate_binary_mask(mask, "Mask");

    cv::Mat output(mask.rows, mask.cols, CV_8UC1);

    for (int row = 0; row < mask.rows; ++row) {
        const auto* mask_row = mask.ptr<std::uint8_t>(row);
        auto* output_row = output.ptr<std::uint8_t>(row);

        for (int col = 0; col < mask.cols; ++col) {
            output_row[col] = mask_row[col] == 0
                ? static_cast<std::uint8_t>(255)
                : static_cast<std::uint8_t>(0);
        }
    }

    return output;
}

cv::Mat MaskOperations::intersection(
    const cv::Mat& first_mask,
    const cv::Mat& second_mask
) const {
    validate_matching_masks(first_mask, second_mask);

    cv::Mat output(first_mask.rows, first_mask.cols, CV_8UC1);

    for (int row = 0; row < first_mask.rows; ++row) {
        const auto* first_row =
            first_mask.ptr<std::uint8_t>(row);
        const auto* second_row =
            second_mask.ptr<std::uint8_t>(row);
        auto* output_row = output.ptr<std::uint8_t>(row);

        for (int col = 0; col < first_mask.cols; ++col) {
            output_row[col] =
                first_row[col] == 255 && second_row[col] == 255
                    ? static_cast<std::uint8_t>(255)
                    : static_cast<std::uint8_t>(0);
        }
    }

    return output;
}

cv::Mat MaskOperations::union_of(
    const cv::Mat& first_mask,
    const cv::Mat& second_mask
) const {
    validate_matching_masks(first_mask, second_mask);

    cv::Mat output(first_mask.rows, first_mask.cols, CV_8UC1);

    for (int row = 0; row < first_mask.rows; ++row) {
        const auto* first_row =
            first_mask.ptr<std::uint8_t>(row);
        const auto* second_row =
            second_mask.ptr<std::uint8_t>(row);
        auto* output_row = output.ptr<std::uint8_t>(row);

        for (int col = 0; col < first_mask.cols; ++col) {
            output_row[col] =
                first_row[col] == 255 || second_row[col] == 255
                    ? static_cast<std::uint8_t>(255)
                    : static_cast<std::uint8_t>(0);
        }
    }

    return output;
}

cv::Mat MaskOperations::difference(
    const cv::Mat& first_mask,
    const cv::Mat& second_mask
) const {
    validate_matching_masks(first_mask, second_mask);

    cv::Mat output(first_mask.rows, first_mask.cols, CV_8UC1);

    for (int row = 0; row < first_mask.rows; ++row) {
        const auto* first_row =
            first_mask.ptr<std::uint8_t>(row);
        const auto* second_row =
            second_mask.ptr<std::uint8_t>(row);
        auto* output_row = output.ptr<std::uint8_t>(row);

        for (int col = 0; col < first_mask.cols; ++col) {
            output_row[col] =
                first_row[col] == 255 && second_row[col] == 0
                    ? static_cast<std::uint8_t>(255)
                    : static_cast<std::uint8_t>(0);
        }
    }

    return output;
}

cv::Mat MaskOperations::apply_to_grayscale(
    const cv::Mat& input_image,
    const cv::Mat& mask
) const {
    validate_image_and_mask(input_image, mask, 1);

    cv::Mat output = cv::Mat::zeros(
        input_image.rows,
        input_image.cols,
        CV_8UC1
    );

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row =
            input_image.ptr<std::uint8_t>(row);
        const auto* mask_row = mask.ptr<std::uint8_t>(row);
        auto* output_row = output.ptr<std::uint8_t>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            if (mask_row[col] == 255) {
                output_row[col] = input_row[col];
            }
        }
    }

    return output;
}

cv::Mat MaskOperations::apply_to_color(
    const cv::Mat& input_image,
    const cv::Mat& mask
) const {
    validate_image_and_mask(input_image, mask, 3);

    cv::Mat output = cv::Mat::zeros(
        input_image.rows,
        input_image.cols,
        CV_8UC3
    );

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<cv::Vec3b>(row);
        const auto* mask_row = mask.ptr<std::uint8_t>(row);
        auto* output_row = output.ptr<cv::Vec3b>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            if (mask_row[col] == 255) {
                output_row[col][0] = input_row[col][0];
                output_row[col][1] = input_row[col][1];
                output_row[col][2] = input_row[col][2];
            }
        }
    }

    return output;
}

} // namespace pdi::segmentation

/**
 * @file binary_morphology.cpp
 * @brief Implements manual binary erosion and dilation.
 */

#include "pdi/morphology/binary_morphology.hpp"

#include <opencv2/core.hpp>

#include <cstdint>
#include <stdexcept>

namespace pdi::morphology {
namespace {

void validate_binary_image(const cv::Mat& image) {
    if (image.empty()) {
        throw std::invalid_argument("Binary image must not be empty.");
    }

    if (image.type() != CV_8UC1) {
        throw std::invalid_argument("Binary image must have type CV_8UC1.");
    }

    for (int row = 0; row < image.rows; ++row) {
        const auto* image_row = image.ptr<std::uint8_t>(row);

        for (int column = 0; column < image.cols; ++column) {
            const std::uint8_t value = image_row[column];

            if (value != 0 && value != 255) {
                throw std::invalid_argument(
                    "Binary image must contain only values 0 and 255."
                );
            }
        }
    }
}

void validate_border_strategy(
    const MorphologyBorderStrategy border_strategy
) {
    if (border_strategy != MorphologyBorderStrategy::OutsideBackground) {
        throw std::invalid_argument("Unsupported border strategy.");
    }
}

[[nodiscard]] bool is_inside(
    const cv::Mat& image,
    const int row,
    const int column
) {
    return row >= 0
        && column >= 0
        && row < image.rows
        && column < image.cols;
}

} // namespace

BinaryStructuringElement BinaryStructuringElement::square_3x3() {
    return {
        .mask = cv::Mat(3, 3, CV_8UC1, cv::Scalar(255)),
        .anchor = {1, 1},
    };
}

BinaryStructuringElement BinaryStructuringElement::cross_3x3() {
    const cv::Mat mask = (
        cv::Mat_<std::uint8_t>(3, 3)
        << 0, 255, 0,
           255, 255, 255,
           0, 255, 0
    );

    return {
        .mask = mask,
        .anchor = {1, 1},
    };
}

void BinaryStructuringElement::validate() const {
    if (mask.empty()) {
        throw std::invalid_argument(
            "Structuring element mask must not be empty."
        );
    }

    if (mask.type() != CV_8UC1) {
        throw std::invalid_argument(
            "Structuring element mask must have type CV_8UC1."
        );
    }

    if (mask.rows != 3 || mask.cols != 3) {
        throw std::invalid_argument(
            "Initial support is restricted to 3 x 3 elements."
        );
    }

    if (anchor.x < 0 || anchor.y < 0
        || anchor.x >= mask.cols || anchor.y >= mask.rows) {
        throw std::invalid_argument(
            "Structuring element anchor must be inside the mask."
        );
    }

    bool has_active_position = false;

    for (int row = 0; row < mask.rows; ++row) {
        const auto* mask_row = mask.ptr<std::uint8_t>(row);

        for (int column = 0; column < mask.cols; ++column) {
            const std::uint8_t value = mask_row[column];

            if (value != 0 && value != 255) {
                throw std::invalid_argument(
                    "Structuring element must contain only 0 and 255."
                );
            }

            has_active_position = has_active_position || value == 255;
        }
    }

    if (!has_active_position) {
        throw std::invalid_argument(
            "Structuring element must contain an active position."
        );
    }
}

cv::Mat BinaryMorphology::erode(
    const cv::Mat& binary_image,
    const BinaryStructuringElement& element,
    const MorphologyBorderStrategy border_strategy
) const {
    validate_binary_image(binary_image);
    element.validate();
    validate_border_strategy(border_strategy);

    cv::Mat output(
        binary_image.rows,
        binary_image.cols,
        CV_8UC1,
        cv::Scalar(0)
    );

    for (int row = 0; row < binary_image.rows; ++row) {
        auto* output_row = output.ptr<std::uint8_t>(row);

        for (int column = 0; column < binary_image.cols; ++column) {
            bool all_foreground = true;

            for (int element_row = 0;
                 element_row < element.mask.rows && all_foreground;
                 ++element_row) {
                const auto* mask_row =
                    element.mask.ptr<std::uint8_t>(element_row);

                for (int element_column = 0;
                     element_column < element.mask.cols;
                     ++element_column) {
                    if (mask_row[element_column] == 0) {
                        continue;
                    }

                    const int image_row =
                        row + element_row - element.anchor.y;
                    const int image_column =
                        column + element_column - element.anchor.x;

                    if (!is_inside(binary_image, image_row, image_column)) {
                        all_foreground = false;
                        break;
                    }

                    const auto* image_row_data =
                        binary_image.ptr<std::uint8_t>(image_row);

                    if (image_row_data[image_column] == 0) {
                        all_foreground = false;
                        break;
                    }
                }
            }

            output_row[column] = all_foreground ? 255 : 0;
        }
    }

    return output;
}

cv::Mat BinaryMorphology::dilate(
    const cv::Mat& binary_image,
    const BinaryStructuringElement& element,
    const MorphologyBorderStrategy border_strategy
) const {
    validate_binary_image(binary_image);
    element.validate();
    validate_border_strategy(border_strategy);

    cv::Mat output(
        binary_image.rows,
        binary_image.cols,
        CV_8UC1,
        cv::Scalar(0)
    );

    for (int row = 0; row < binary_image.rows; ++row) {
        auto* output_row = output.ptr<std::uint8_t>(row);

        for (int column = 0; column < binary_image.cols; ++column) {
            bool any_foreground = false;

            for (int element_row = 0;
                 element_row < element.mask.rows && !any_foreground;
                 ++element_row) {
                const auto* mask_row =
                    element.mask.ptr<std::uint8_t>(element_row);

                for (int element_column = 0;
                     element_column < element.mask.cols;
                     ++element_column) {
                    if (mask_row[element_column] == 0) {
                        continue;
                    }

                    const int image_row =
                        row + element_row - element.anchor.y;
                    const int image_column =
                        column + element_column - element.anchor.x;

                    if (!is_inside(binary_image, image_row, image_column)) {
                        continue;
                    }

                    const auto* image_row_data =
                        binary_image.ptr<std::uint8_t>(image_row);

                    if (image_row_data[image_column] == 255) {
                        any_foreground = true;
                        break;
                    }
                }
            }

            output_row[column] = any_foreground ? 255 : 0;
        }
    }

    return output;
}

} // namespace pdi::morphology

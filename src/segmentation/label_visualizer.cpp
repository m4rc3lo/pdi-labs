/**
 * @file label_visualizer.cpp
 * @brief Implements visualization and validation utilities for label maps.
 */

#include "pdi/segmentation/label_visualizer.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <cstdint>
#include <stdexcept>
#include <unordered_map>

namespace pdi::segmentation {
namespace {

void validate_labels(const cv::Mat& labels) {
    if (labels.empty()) {
        throw std::invalid_argument("Label matrix must not be empty.");
    }

    if (labels.type() != CV_32SC1) {
        throw std::invalid_argument("Label matrix must have type CV_32SC1.");
    }
}

cv::Vec3b color_from_label(const int label) {
    if (label <= 0) {
        return {0, 0, 0};
    }

    const std::uint32_t value =
        static_cast<std::uint32_t>(label) * 2654435761u;

    const auto blue = static_cast<std::uint8_t>(40u + (value & 0x7Fu));
    const auto green =
        static_cast<std::uint8_t>(40u + ((value >> 8) & 0x7Fu));
    const auto red =
        static_cast<std::uint8_t>(40u + ((value >> 16) & 0x7Fu));

    return {blue, green, red};
}

bool labels_are_equal(
    const cv::Mat& first,
    const cv::Mat& second
) {
    if (first.rows != second.rows || first.cols != second.cols) {
        return false;
    }

    for (int row = 0; row < first.rows; ++row) {
        const auto* first_row = first.ptr<int>(row);
        const auto* second_row = second.ptr<int>(row);

        for (int column = 0; column < first.cols; ++column) {
            if (first_row[column] != second_row[column]) {
                return false;
            }
        }
    }

    return true;
}

} // namespace

cv::Mat LabelVisualizer::colorize(const cv::Mat& labels) const {
    validate_labels(labels);

    cv::Mat colored(labels.rows, labels.cols, CV_8UC3, cv::Scalar(0, 0, 0));

    for (int row = 0; row < labels.rows; ++row) {
        const auto* label_row = labels.ptr<int>(row);
        auto* color_row = colored.ptr<cv::Vec3b>(row);

        for (int column = 0; column < labels.cols; ++column) {
            color_row[column] = color_from_label(label_row[column]);
        }
    }

    return colored;
}

cv::Mat LabelVisualizer::normalize_labels(const cv::Mat& labels) const {
    validate_labels(labels);

    cv::Mat normalized(labels.rows, labels.cols, CV_32SC1, cv::Scalar(0));
    std::unordered_map<int, int> mapping;
    int next_label = 1;

    for (int row = 0; row < labels.rows; ++row) {
        const auto* label_row = labels.ptr<int>(row);
        auto* normalized_row = normalized.ptr<int>(row);

        for (int column = 0; column < labels.cols; ++column) {
            const int label = label_row[column];

            if (label == 0) {
                normalized_row[column] = 0;
                continue;
            }

            if (label < 0) {
                throw std::invalid_argument(
                    "Label matrix must not contain negative labels."
                );
            }

            const auto [iterator, inserted] =
                mapping.emplace(label, next_label);

            if (inserted) {
                ++next_label;
            }

            normalized_row[column] = iterator->second;
        }
    }

    return normalized;
}

LabelValidationSummary LabelVisualizer::compare_with_opencv(
    const cv::Mat& binary_image,
    const cv::Mat& manual_labels,
    const Connectivity connectivity
) const {
    validate_labels(manual_labels);

    if (binary_image.empty()) {
        throw std::invalid_argument("Binary image must not be empty.");
    }

    if (binary_image.type() != CV_8UC1) {
        throw std::invalid_argument("Binary image must have type CV_8UC1.");
    }

    if (binary_image.rows != manual_labels.rows
        || binary_image.cols != manual_labels.cols) {
        throw std::invalid_argument(
            "Binary image and manual labels must have the same dimensions."
        );
    }

    cv::Mat opencv_labels;
    const int opencv_component_count = cv::connectedComponents(
        binary_image,
        opencv_labels,
        connectivity == Connectivity::Four ? 4 : 8,
        CV_32S
    ) - 1;

    int manual_component_count = 0;

    for (int row = 0; row < manual_labels.rows; ++row) {
        const auto* label_row = manual_labels.ptr<int>(row);

        for (int column = 0; column < manual_labels.cols; ++column) {
            manual_component_count =
                std::max(manual_component_count, label_row[column]);
        }
    }

    const cv::Mat manual_normalized = normalize_labels(manual_labels);
    const cv::Mat opencv_normalized = normalize_labels(opencv_labels);

    return {
        .compared_with_opencv = true,
        .manual_component_count = manual_component_count,
        .opencv_component_count = opencv_component_count,
        .same_partition = labels_are_equal(
            manual_normalized,
            opencv_normalized
        ),
    };
}

} // namespace pdi::segmentation

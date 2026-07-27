/**
 * @file component_feature_extractor.cpp
 * @brief Implements manual connected-component feature extraction.
 */

#include "pdi/segmentation/component_feature_extractor.hpp"

#include <opencv2/core.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace pdi::segmentation {
namespace {

struct ComponentAccumulator {
    int area = 0;
    int minimum_row = std::numeric_limits<int>::max();
    int minimum_column = std::numeric_limits<int>::max();
    int maximum_row = -1;
    int maximum_column = -1;
    std::int64_t row_sum = 0;
    std::int64_t column_sum = 0;
};

void validate_labels(const cv::Mat& labels) {
    if (labels.empty()) {
        throw std::invalid_argument("Label matrix must not be empty.");
    }

    if (labels.type() != CV_32SC1) {
        throw std::invalid_argument("Label matrix must have type CV_32SC1.");
    }
}

} // namespace

std::vector<ComponentFeatures> ComponentFeatureExtractor::extract(
    const cv::Mat& labels
) const {
    validate_labels(labels);

    int maximum_label = 0;

    for (int row = 0; row < labels.rows; ++row) {
        const auto* label_row = labels.ptr<int>(row);

        for (int column = 0; column < labels.cols; ++column) {
            const int label = label_row[column];

            if (label < 0) {
                throw std::invalid_argument(
                    "Label matrix must not contain negative labels."
                );
            }

            maximum_label = std::max(maximum_label, label);
        }
    }

    std::vector<ComponentAccumulator> accumulators(
        static_cast<std::size_t>(maximum_label + 1)
    );

    for (int row = 0; row < labels.rows; ++row) {
        const auto* label_row = labels.ptr<int>(row);

        for (int column = 0; column < labels.cols; ++column) {
            const int label = label_row[column];

            if (label == 0) {
                continue;
            }

            auto& accumulator =
                accumulators[static_cast<std::size_t>(label)];

            ++accumulator.area;
            accumulator.minimum_row =
                std::min(accumulator.minimum_row, row);
            accumulator.minimum_column =
                std::min(accumulator.minimum_column, column);
            accumulator.maximum_row =
                std::max(accumulator.maximum_row, row);
            accumulator.maximum_column =
                std::max(accumulator.maximum_column, column);
            accumulator.row_sum += row;
            accumulator.column_sum += column;
        }
    }

    std::vector<ComponentFeatures> features;

    for (int label = 1; label <= maximum_label; ++label) {
        const auto& accumulator =
            accumulators[static_cast<std::size_t>(label)];

        if (accumulator.area == 0) {
            continue;
        }

        features.push_back({
            .label = label,
            .area = accumulator.area,
            .bounding_box = {
                accumulator.minimum_column,
                accumulator.minimum_row,
                accumulator.maximum_column
                    - accumulator.minimum_column + 1,
                accumulator.maximum_row
                    - accumulator.minimum_row + 1,
            },
            .centroid = {
                static_cast<double>(accumulator.column_sum)
                    / static_cast<double>(accumulator.area),
                static_cast<double>(accumulator.row_sum)
                    / static_cast<double>(accumulator.area),
            },
        });
    }

    return features;
}

pdi::io::ProcessingRecord
ComponentFeatureExtractor::make_processing_record(
    const cv::Mat& labels,
    const std::vector<ComponentFeatures>& features,
    const ComponentFeatureRecordContext& context
) const {
    validate_labels(labels);

    if (context.format_version.empty()
        || context.project_version.empty()
        || context.laboratory.empty()
        || context.operation.empty()
        || context.input_path.empty()
        || context.connectivity.empty()) {
        throw std::invalid_argument(
            "Component feature record context fields must not be empty."
        );
    }

    int total_foreground_area = 0;
    int largest_component_label = 0;
    int largest_component_area = 0;

    for (const auto& feature : features) {
        if (feature.label <= 0 || feature.area <= 0) {
            throw std::invalid_argument(
                "Component features must use positive labels and areas."
            );
        }

        total_foreground_area += feature.area;

        if (feature.area > largest_component_area) {
            largest_component_area = feature.area;
            largest_component_label = feature.label;
        }
    }

    return {
        .format_version = context.format_version,
        .project_version = context.project_version,
        .laboratory = context.laboratory,
        .operation = context.operation,
        .input_path = context.input_path,
        .parameters = {
            {"connectivity", context.connectivity},
            {"component_count", std::to_string(features.size())},
            {"statistic.total_foreground_area",
             std::to_string(total_foreground_area)},
            {"statistic.largest_component_label",
             std::to_string(largest_component_label)},
            {"statistic.largest_component_area",
             std::to_string(largest_component_area)},
            {"statistic.image_rows", std::to_string(labels.rows)},
            {"statistic.image_columns", std::to_string(labels.cols)},
        },
        .numeric_artifacts = {
            {"labels", labels.clone()},
        },
    };
}

} // namespace pdi::segmentation

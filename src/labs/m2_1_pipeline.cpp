/**
 * @file m2_1_pipeline.cpp
 * @brief Implements the integrated Laboratory M2.1 processing pipeline.
 */

#include "pdi/labs/m2_1_pipeline.hpp"

#include "pdi/segmentation/library_segmentation_pipeline.hpp"
#include "pdi/segmentation/manual_threshold.hpp"
#include "pdi/segmentation/mask_operations.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

std::string integer_value(int value) {
    return std::to_string(value);
}

std::string floating_value(double value) {
    return std::to_string(value);
}

pdi::segmentation::LibrarySegmentationOperation library_operation(
    pdi::labs::M21Operation operation
) {
    using M21Operation = pdi::labs::M21Operation;
    using LibraryOperation =
        pdi::segmentation::LibrarySegmentationOperation;

    switch (operation) {
    case M21Operation::BgrToHsv:
        return LibraryOperation::BgrToHsv;
    case M21Operation::Otsu:
        return LibraryOperation::Otsu;
    case M21Operation::AdaptiveMean:
        return LibraryOperation::AdaptiveMean;
    case M21Operation::DistanceTransform:
        return LibraryOperation::DistanceTransform;
    case M21Operation::Watershed:
        return LibraryOperation::Watershed;
    default:
        throw std::invalid_argument(
            "Operation is not library based."
        );
    }
}

} // namespace

namespace pdi::labs {

M21PipelineResult M21Pipeline::run(
    const cv::Mat& input_image,
    const cv::Mat& secondary_image,
    const M21Parameters& parameters
) const {
    if (parameters.threshold < 0 || parameters.threshold > 255) {
        throw std::invalid_argument("Threshold must be in [0, 255].");
    }
    if (parameters.minimum_value < 0
        || parameters.minimum_value > 255
        || parameters.maximum_value < 0
        || parameters.maximum_value > 255
        || parameters.minimum_value > parameters.maximum_value) {
        throw std::invalid_argument(
            "Manual interval must satisfy 0 <= minimum <= maximum <= 255."
        );
    }

    const pdi::segmentation::ManualThreshold thresholding;
    const pdi::segmentation::MaskOperations masks;

    switch (parameters.operation) {
    case M21Operation::ManualGlobal: {
        const cv::Mat output = thresholding.binary_global(
            input_image,
            static_cast<std::uint8_t>(parameters.threshold)
        );
        return {
            .operation_name = "manual_global",
            .visual_outputs = {{"manual_global_binary", output}},
            .parameters = {
                {"threshold", integer_value(parameters.threshold)},
                {"comparison", "value>=threshold"},
                {"foreground", "255"},
                {"background", "0"},
            },
            .numeric_artifacts = {},
        };
    }

    case M21Operation::ManualInterval: {
        const cv::Mat output = thresholding.select_interval(
            input_image,
            static_cast<std::uint8_t>(parameters.minimum_value),
            static_cast<std::uint8_t>(parameters.maximum_value)
        );
        return {
            .operation_name = "manual_interval",
            .visual_outputs = {{"manual_interval_binary", output}},
            .parameters = {
                {"minimum", integer_value(parameters.minimum_value)},
                {"maximum", integer_value(parameters.maximum_value)},
                {"interval", "inclusive"},
                {"foreground", "255"},
                {"background", "0"},
            },
            .numeric_artifacts = {},
        };
    }

    case M21Operation::MaskInvert:
        return {
            .operation_name = "mask_invert",
            .visual_outputs = {{"mask_inverted", masks.invert(input_image)}},
            .parameters = {{"binary_values", "0,255"}},
            .numeric_artifacts = {},
        };

    case M21Operation::MaskIntersection:
        return {
            .operation_name = "mask_intersection",
            .visual_outputs = {{
                "mask_intersection",
                masks.intersection(input_image, secondary_image),
            }},
            .parameters = {{"binary_values", "0,255"}},
            .numeric_artifacts = {},
        };

    case M21Operation::MaskUnion:
        return {
            .operation_name = "mask_union",
            .visual_outputs = {{
                "mask_union",
                masks.union_of(input_image, secondary_image),
            }},
            .parameters = {{"binary_values", "0,255"}},
            .numeric_artifacts = {},
        };

    case M21Operation::MaskDifference:
        return {
            .operation_name = "mask_difference",
            .visual_outputs = {{
                "mask_difference",
                masks.difference(input_image, secondary_image),
            }},
            .parameters = {
                {"direction", "primary\\secondary"},
                {"binary_values", "0,255"},
            },
            .numeric_artifacts = {},
        };

    case M21Operation::MaskApplyGrayscale:
        return {
            .operation_name = "mask_apply_grayscale",
            .visual_outputs = {{
                "masked_grayscale",
                masks.apply_to_grayscale(input_image, secondary_image),
            }},
            .parameters = {{"masked_value", "0"}},
            .numeric_artifacts = {},
        };

    case M21Operation::MaskApplyColor:
        return {
            .operation_name = "mask_apply_color",
            .visual_outputs = {{
                "masked_color",
                masks.apply_to_color(input_image, secondary_image),
            }},
            .parameters = {{"masked_value", "black"}},
            .numeric_artifacts = {},
        };

    case M21Operation::BgrToHsv:
    case M21Operation::Otsu:
    case M21Operation::AdaptiveMean:
    case M21Operation::DistanceTransform:
    case M21Operation::Watershed: {
        const auto library_result =
            pdi::segmentation::LibrarySegmentationPipeline{}.run(
                input_image,
                {
                    .operation = library_operation(parameters.operation),
                    .adaptive_block_size =
                        parameters.adaptive_block_size,
                    .adaptive_constant = parameters.adaptive_constant,
                    .distance_mask_size = static_cast<double>(
                        parameters.distance_mask_size
                    ),
                    .foreground_ratio = parameters.foreground_ratio,
                }
            );

        std::vector<M21VisualOutput> outputs;
        outputs.reserve(library_result.visual_outputs.size());
        for (const auto& output : library_result.visual_outputs) {
            outputs.push_back({output.name, output.image});
        }

        auto result_parameters = library_result.parameters;
        result_parameters.push_back({
            "configured_adaptive_block_size",
            integer_value(parameters.adaptive_block_size),
        });
        result_parameters.push_back({
            "configured_adaptive_constant",
            floating_value(parameters.adaptive_constant),
        });
        result_parameters.push_back({
            "configured_distance_mask_size",
            integer_value(parameters.distance_mask_size),
        });
        result_parameters.push_back({
            "configured_foreground_ratio",
            floating_value(parameters.foreground_ratio),
        });

        return {
            .operation_name = library_result.operation_name,
            .visual_outputs = std::move(outputs),
            .parameters = std::move(result_parameters),
            .numeric_artifacts = library_result.numeric_artifacts,
        };
    }
    }

    throw std::invalid_argument("Unsupported M2.1 operation.");
}

bool M21Pipeline::requires_color_input(M21Operation operation) {
    return operation == M21Operation::MaskApplyColor
        || operation == M21Operation::BgrToHsv
        || operation == M21Operation::Watershed;
}

bool M21Pipeline::requires_secondary_input(M21Operation operation) {
    return operation == M21Operation::MaskIntersection
        || operation == M21Operation::MaskUnion
        || operation == M21Operation::MaskDifference
        || operation == M21Operation::MaskApplyGrayscale
        || operation == M21Operation::MaskApplyColor;
}

} // namespace pdi::labs

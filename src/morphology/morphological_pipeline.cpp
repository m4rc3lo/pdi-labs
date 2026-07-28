/**
 * @file morphological_pipeline.cpp
 * @brief Implements a configurable and traceable morphology pipeline.
 */

#include "pdi/morphology/morphological_pipeline.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace pdi::morphology {
namespace {

[[nodiscard]] int foreground_area(const cv::Mat& image) {
    int area = 0;

    for (int row = 0; row < image.rows; ++row) {
        const auto* image_row = image.ptr<std::uint8_t>(row);

        for (int column = 0; column < image.cols; ++column) {
            area += image_row[column] == 255 ? 1 : 0;
        }
    }

    return area;
}

[[nodiscard]] cv::Mat apply_manual(
    const BinaryMorphology& morphology,
    const cv::Mat& input,
    const MorphologicalOperation operation,
    const BinaryStructuringElement& element,
    const MorphologyBorderStrategy border_strategy
) {
    switch (operation) {
    case MorphologicalOperation::Erode:
        return morphology.erode(input, element, border_strategy);
    case MorphologicalOperation::Dilate:
        return morphology.dilate(input, element, border_strategy);
    case MorphologicalOperation::Open:
        return morphology.open(input, element, border_strategy);
    case MorphologicalOperation::Close:
        return morphology.close(input, element, border_strategy);
    }

    throw std::invalid_argument("Unsupported morphology operation.");
}

[[nodiscard]] cv::Mat apply_opencv(
    const cv::Mat& input,
    const MorphologicalOperation operation,
    const BinaryStructuringElement& element
) {
    cv::Mat output;

    switch (operation) {
    case MorphologicalOperation::Erode:
        cv::erode(
            input,
            output,
            element.mask,
            element.anchor,
            1,
            cv::BORDER_CONSTANT,
            cv::Scalar(0)
        );
        break;
    case MorphologicalOperation::Dilate:
        cv::dilate(
            input,
            output,
            element.mask,
            element.anchor,
            1,
            cv::BORDER_CONSTANT,
            cv::Scalar(0)
        );
        break;
    case MorphologicalOperation::Open:
        cv::morphologyEx(
            input,
            output,
            cv::MORPH_OPEN,
            element.mask,
            element.anchor,
            1,
            cv::BORDER_CONSTANT,
            cv::Scalar(0)
        );
        break;
    case MorphologicalOperation::Close:
        cv::morphologyEx(
            input,
            output,
            cv::MORPH_CLOSE,
            element.mask,
            element.anchor,
            1,
            cv::BORDER_CONSTANT,
            cv::Scalar(0)
        );
        break;
    }

    return output;
}

[[nodiscard]] bool matrices_equal(
    const cv::Mat& first,
    const cv::Mat& second
) {
    if (first.type() != second.type() || first.size() != second.size()) {
        return false;
    }

    for (int row = 0; row < first.rows; ++row) {
        const auto* first_row = first.ptr<std::uint8_t>(row);
        const auto* second_row = second.ptr<std::uint8_t>(row);

        for (int column = 0; column < first.cols; ++column) {
            if (first_row[column] != second_row[column]) {
                return false;
            }
        }
    }

    return true;
}

[[nodiscard]] std::vector<int> removed_labels(
    const cv::Mat& initial_labels,
    const cv::Mat& final_image,
    const int initial_component_count
) {
    std::unordered_set<int> surviving_labels;

    for (int row = 0; row < initial_labels.rows; ++row) {
        const auto* label_row = initial_labels.ptr<int>(row);
        const auto* final_row = final_image.ptr<std::uint8_t>(row);

        for (int column = 0; column < initial_labels.cols; ++column) {
            if (label_row[column] > 0 && final_row[column] == 255) {
                surviving_labels.insert(label_row[column]);
            }
        }
    }

    std::vector<int> removed;

    for (int label = 1; label <= initial_component_count; ++label) {
        if (!surviving_labels.contains(label)) {
            removed.push_back(label);
        }
    }

    return removed;
}

[[nodiscard]] std::string integer_list(
    const std::vector<int>& values
) {
    std::ostringstream stream;

    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index > 0) {
            stream << ',';
        }

        stream << values[index];
    }

    return stream.str();
}

} // namespace

MorphologicalPipelineConfig
MorphologicalPipelineConfig::noise_removal_3x3() {
    return {
        .element = BinaryStructuringElement::square_3x3(),
        .sequence = {
            MorphologicalOperation::Erode,
            MorphologicalOperation::Dilate,
        },
    };
}

MorphologicalPipelineConfig
MorphologicalPipelineConfig::hole_filling_3x3() {
    return {
        .element = BinaryStructuringElement::square_3x3(),
        .sequence = {
            MorphologicalOperation::Dilate,
            MorphologicalOperation::Erode,
        },
    };
}

MorphologicalPipelineResult MorphologicalPipeline::run(
    const cv::Mat& binary_image,
    const MorphologicalPipelineConfig& config
) const {
    if (config.sequence.size() < 2) {
        throw std::invalid_argument(
            "Morphological pipeline requires at least two operations."
        );
    }

    config.element.validate();

    const pdi::segmentation::ConnectedComponentLabeler labeler;
    const auto initial_labeling = labeler.label(
        binary_image,
        config.connectivity
    );

    const BinaryMorphology morphology;
    cv::Mat current = binary_image.clone();
    std::vector<cv::Mat> intermediate_images;
    intermediate_images.reserve(config.sequence.size());

    for (const MorphologicalOperation operation : config.sequence) {
        current = apply_manual(
            morphology,
            current,
            operation,
            config.element,
            config.border_strategy
        );
        intermediate_images.push_back(current.clone());
    }

    const auto final_labeling = labeler.label(
        current,
        config.connectivity
    );

    const std::vector<int> removed = removed_labels(
        initial_labeling.labels,
        current,
        initial_labeling.component_count
    );

    bool matches_opencv = false;

    if (config.compare_with_opencv) {
        cv::Mat opencv_current = binary_image.clone();

        for (const MorphologicalOperation operation : config.sequence) {
            opencv_current = apply_opencv(
                opencv_current,
                operation,
                config.element
            );
        }

        matches_opencv = matrices_equal(current, opencv_current);
    }

    return {
        .input = binary_image.clone(),
        .output = current,
        .initial_labels = initial_labeling.labels,
        .final_labels = final_labeling.labels,
        .intermediate_images = std::move(intermediate_images),
        .foreground_area_before = foreground_area(binary_image),
        .foreground_area_after = foreground_area(current),
        .component_count_before = initial_labeling.component_count,
        .component_count_after = final_labeling.component_count,
        .removed_component_count = static_cast<int>(removed.size()),
        .removed_component_labels = removed,
        .compared_with_opencv = config.compare_with_opencv,
        .matches_opencv = matches_opencv,
    };
}

pdi::io::ProcessingRecord MorphologicalPipeline::make_processing_record(
    const MorphologicalPipelineResult& result,
    const MorphologicalPipelineConfig& config,
    const std::string& project_version,
    const std::string& input_path
) const {
    pdi::io::ProcessingRecord record{
        .format_version = "1",
        .project_version = project_version,
        .laboratory = "M2.3",
        .operation = "morphological_pipeline",
        .input_path = input_path,
    };

    record.parameters = {
        {"sequence", sequence_name(config.sequence)},
        {
            "border_strategy",
            config.border_strategy
                    == MorphologyBorderStrategy::OutsideBackground
                ? "outside_background"
                : "unsupported"
        },
        {
            "connectivity",
            config.connectivity
                    == pdi::segmentation::Connectivity::Four
                ? "4"
                : "8"
        },
        {
            "foreground_area_before",
            std::to_string(result.foreground_area_before)
        },
        {
            "foreground_area_after",
            std::to_string(result.foreground_area_after)
        },
        {
            "component_count_before",
            std::to_string(result.component_count_before)
        },
        {
            "component_count_after",
            std::to_string(result.component_count_after)
        },
        {
            "removed_component_count",
            std::to_string(result.removed_component_count)
        },
        {
            "removed_component_labels",
            integer_list(result.removed_component_labels)
        },
        {
            "opencv_validation_enabled",
            result.compared_with_opencv ? "true" : "false"
        },
        {
            "matches_opencv",
            result.matches_opencv ? "true" : "false"
        },
    };

    record.numeric_artifacts.push_back({
        "structuring_element",
        config.element.mask.clone(),
    });
    record.numeric_artifacts.push_back({
        "input",
        result.input.clone(),
    });

    for (std::size_t index = 0;
         index < result.intermediate_images.size();
         ++index) {
        record.numeric_artifacts.push_back({
            "stage_" + std::to_string(index + 1)
                + "_" + operation_name(config.sequence[index]),
            result.intermediate_images[index].clone(),
        });
    }

    record.numeric_artifacts.push_back({
        "output",
        result.output.clone(),
    });
    record.numeric_artifacts.push_back({
        "initial_labels",
        result.initial_labels.clone(),
    });
    record.numeric_artifacts.push_back({
        "final_labels",
        result.final_labels.clone(),
    });

    return record;
}

std::string MorphologicalPipeline::operation_name(
    const MorphologicalOperation operation
) {
    switch (operation) {
    case MorphologicalOperation::Erode:
        return "erode";
    case MorphologicalOperation::Dilate:
        return "dilate";
    case MorphologicalOperation::Open:
        return "open";
    case MorphologicalOperation::Close:
        return "close";
    }

    throw std::invalid_argument("Unsupported morphology operation.");
}

std::string MorphologicalPipeline::sequence_name(
    const std::vector<MorphologicalOperation>& sequence
) {
    std::ostringstream stream;

    for (std::size_t index = 0; index < sequence.size(); ++index) {
        if (index > 0) {
            stream << " -> ";
        }

        stream << operation_name(sequence[index]);
    }

    return stream.str();
}

} // namespace pdi::morphology

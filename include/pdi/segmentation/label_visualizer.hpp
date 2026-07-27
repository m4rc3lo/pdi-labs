/**
 * @file label_visualizer.hpp
 * @brief Declares visualization and optional OpenCV validation for label maps.
 */

#pragma once

#include "pdi/segmentation/connected_component_labeler.hpp"

#include <opencv2/core/mat.hpp>

namespace pdi::segmentation {

/**
 * @brief Summarizes an optional comparison against OpenCV.
 */
struct LabelValidationSummary {
    bool compared_with_opencv = false;
    int manual_component_count = 0;
    int opencv_component_count = 0;
    bool same_partition = false;
};

/**
 * @brief Creates color visualizations for connected-component labels and
 * compares them to OpenCV when requested.
 *
 * @details Label zero is always mapped to black. Positive labels are mapped to
 * deterministic pseudo-colors so that repeated executions generate the same
 * `labels.png`.
 *
 * For validation, OpenCV may assign different numeric identifiers than the
 * manual algorithm. Therefore, the comparison normalizes both label matrices by
 * first occurrence and compares the resulting partitions instead of raw label
 * values.
 */
class LabelVisualizer {
public:
    /**
     * @brief Converts a `CV_32SC1` label matrix into a `CV_8UC3` image.
     *
     * @param labels Label matrix where zero represents the background.
     * @return BGR image suitable for visualization and persistence.
     *
     * @throws std::invalid_argument If the input is empty or not `CV_32SC1`.
     */
    [[nodiscard]] cv::Mat colorize(const cv::Mat& labels) const;

    /**
     * @brief Normalizes labels by first occurrence.
     *
     * @param labels Label matrix with type `CV_32SC1`.
     * @return Matrix with background preserved at zero and positive labels
     * renumbered deterministically from one.
     */
    [[nodiscard]] cv::Mat normalize_labels(const cv::Mat& labels) const;

    /**
     * @brief Optionally compares the manual result with OpenCV.
     *
     * @param binary_image Binary `CV_8UC1` image used to generate the labels.
     * @param manual_labels Manual label matrix with type `CV_32SC1`.
     * @param connectivity Connectivity used by the manual algorithm.
     * @return Summary including component counts and partition equivalence.
     *
     * @details This function may use `cv::connectedComponents` only for
     * validation. Differences may still appear in raw numeric labels because
     * label numbering order is implementation-defined. The boolean
     * `same_partition` reflects the comparison after normalization.
     */
    [[nodiscard]] LabelValidationSummary compare_with_opencv(
        const cv::Mat& binary_image,
        const cv::Mat& manual_labels,
        Connectivity connectivity
    ) const;
};

} // namespace pdi::segmentation

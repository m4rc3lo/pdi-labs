/**
 * @file component_feature_extractor.hpp
 * @brief Declares manual feature extraction from connected-component labels.
 */

#pragma once

#include "pdi/io/processing_record.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#include <string>
#include <vector>

namespace pdi::segmentation {

/**
 * @brief Stores geometric characteristics of one connected component.
 */
struct ComponentFeatures {
    int label = 0;
    int area = 0;
    cv::Rect bounding_box;
    cv::Point2d centroid;
};

/**
 * @brief Supplies execution metadata for a component feature record.
 */
struct ComponentFeatureRecordContext {
    std::string format_version = "1";
    std::string project_version;
    std::string laboratory = "M2.2";
    std::string operation = "component_features";
    std::string input_path;
    std::string connectivity;
};

/**
 * @brief Extracts area, bounding box and centroid from a label matrix.
 *
 * @details The input must have type `CV_32SC1`. Label zero is reserved for
 * background and is ignored. Positive labels do not need to be consecutive.
 * Results are returned in increasing label order.
 *
 * The implementation explicitly traverses rows with `cv::Mat::ptr<int>`.
 * For an `M x N` matrix and `K` labels, time complexity is `O(MN + K)` and
 * auxiliary memory is `O(K)`.
 */
class ComponentFeatureExtractor {
public:
    /**
     * @brief Computes one feature record for each positive label.
     *
     * @param labels Label matrix with type `CV_32SC1`.
     * @return Component records ordered by increasing label.
     *
     * @throws std::invalid_argument If the matrix is empty, has another type or
     * contains negative labels.
     */
    [[nodiscard]] std::vector<ComponentFeatures> extract(
        const cv::Mat& labels
    ) const;

    /**
     * @brief Creates a ProcessingRecord complementary to the CSV export.
     *
     * @details The record stores execution parameters and aggregate statistics
     * as named values, and preserves the original label matrix as the numeric
     * artifact `labels`. CSV remains the tabular component-level output.
     */
    [[nodiscard]] pdi::io::ProcessingRecord make_processing_record(
        const cv::Mat& labels,
        const std::vector<ComponentFeatures>& features,
        const ComponentFeatureRecordContext& context
    ) const;
};

} // namespace pdi::segmentation

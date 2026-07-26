/**
 * @file processing_record.hpp
 * @brief Declares generic records for processing traceability.
 */

#pragma once

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>

namespace pdi::io {

/**
 * @brief Associates a textual parameter name with its serialized value.
 */
struct NamedValue {
    std::string name;
    std::string value;
};

/**
 * @brief Associates a stable artifact name with one numeric matrix.
 *
 * @note The storage layer writes the matrix with its original OpenCV type.
 * Callers should clone matrices when independent ownership is required.
 */
struct NamedMatrix {
    std::string name;
    cv::Mat value;
};

/**
 * @brief Describes one reproducible processing execution.
 *
 * @details The record is independent of individual laboratories and may contain
 * only metadata and parameters when no numeric matrix is pertinent.
 */
struct ProcessingRecord {
    std::string format_version;
    std::string project_version;
    std::string laboratory;
    std::string operation;
    std::string input_path;
    std::vector<NamedValue> parameters;
    std::vector<NamedMatrix> numeric_artifacts;
};

} // namespace pdi::io

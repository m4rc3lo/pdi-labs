/**
 * @file processing_data_storage.hpp
 * @brief Declares YAML persistence for generic processing records.
 */

#pragma once

#include "pdi/io/processing_record.hpp"

#include <filesystem>

namespace pdi::io {

/**
 * @brief Saves and loads processing traceability records with OpenCV YAML.
 *
 * @details The class has no knowledge of laboratory-specific operations. File
 * names and batching policies remain responsibilities of the caller.
 */
class ProcessingDataStorage {
public:
    /**
     * @brief Saves one processing record as YAML.
     *
     * @param output_path Destination ending in `.yml` or `.yaml`.
     * @param record Record containing metadata, parameters and numeric matrices.
     *
     * @throws std::invalid_argument If required fields, names or matrices are
     * invalid.
     * @throws std::runtime_error If directories or the YAML file cannot be
     * created.
     */
    void save_yaml(
        const std::filesystem::path& output_path,
        const ProcessingRecord& record
    ) const;

    /**
     * @brief Loads one processing record from YAML.
     *
     * @param input_path Existing `.yml` or `.yaml` file.
     * @return Reconstructed record with matrices preserving their OpenCV types.
     *
     * @throws std::invalid_argument If the extension is unsupported.
     * @throws std::runtime_error If the file cannot be opened or is malformed.
     */
    [[nodiscard]] ProcessingRecord load_yaml(
        const std::filesystem::path& input_path
    ) const;
};

} // namespace pdi::io

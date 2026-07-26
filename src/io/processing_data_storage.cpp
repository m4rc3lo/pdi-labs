/**
 * @file processing_data_storage.cpp
 * @brief Implements YAML persistence for generic processing records.
 */

#include "pdi/io/processing_data_storage.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/persistence.hpp>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace {

void require_yaml_extension(const std::filesystem::path& path) {
    const std::string extension = path.extension().string();

    if (extension != ".yml" && extension != ".yaml") {
        throw std::invalid_argument(
            "Processing data storage requires a .yml or .yaml file: "
            + path.string()
        );
    }
}

void require_non_empty(
    const std::string& value,
    const std::string& field_name
) {
    if (value.empty()) {
        throw std::invalid_argument(
            "Processing record field must not be empty: " + field_name
        );
    }
}

void validate_record(const pdi::io::ProcessingRecord& record) {
    require_non_empty(record.format_version, "format_version");
    require_non_empty(record.project_version, "project_version");
    require_non_empty(record.laboratory, "laboratory");
    require_non_empty(record.operation, "operation");
    require_non_empty(record.input_path, "input_path");

    std::unordered_set<std::string> parameter_names;

    for (const auto& parameter : record.parameters) {
        require_non_empty(parameter.name, "parameter.name");

        if (!parameter_names.insert(parameter.name).second) {
            throw std::invalid_argument(
                "Duplicate processing parameter: " + parameter.name
            );
        }
    }

    std::unordered_set<std::string> artifact_names;

    for (const auto& artifact : record.numeric_artifacts) {
        require_non_empty(artifact.name, "numeric_artifact.name");

        if (artifact.value.empty()) {
            throw std::invalid_argument(
                "Numeric artifact matrix must not be empty: " + artifact.name
            );
        }

        if (!artifact_names.insert(artifact.name).second) {
            throw std::invalid_argument(
                "Duplicate numeric artifact: " + artifact.name
            );
        }
    }
}

[[nodiscard]] std::string read_required_string(
    const cv::FileStorage& storage,
    const std::string& field_name
) {
    const cv::FileNode node = storage[field_name];

    if (node.empty() || !node.isString()) {
        throw std::runtime_error(
            "Required YAML string field is missing or invalid: " + field_name
        );
    }

    const std::string value = static_cast<std::string>(node);

    if (value.empty()) {
        throw std::runtime_error(
            "Required YAML string field is empty: " + field_name
        );
    }

    return value;
}

} // namespace

namespace pdi::io {

void ProcessingDataStorage::save_yaml(
    const std::filesystem::path& output_path,
    const ProcessingRecord& record
) const {
    require_yaml_extension(output_path);
    validate_record(record);

    std::error_code error;
    const auto parent_path = output_path.parent_path();

    if (!parent_path.empty()) {
        std::filesystem::create_directories(parent_path, error);

        if (error) {
            throw std::runtime_error(
                "Could not create processing data directory: "
                + parent_path.string() + ": " + error.message()
            );
        }
    }

    try {
        cv::FileStorage storage(
            output_path.string(),
            cv::FileStorage::WRITE | cv::FileStorage::FORMAT_YAML
        );

        if (!storage.isOpened()) {
            throw std::runtime_error(
                "Could not open processing data file for writing: "
                + output_path.string()
            );
        }

        storage << "format_version" << record.format_version;
        storage << "project_version" << record.project_version;
        storage << "laboratory" << record.laboratory;
        storage << "operation" << record.operation;
        storage << "input_path" << record.input_path;

        storage << "parameters" << "[";
        for (const auto& parameter : record.parameters) {
            storage << "{";
            storage << "name" << parameter.name;
            storage << "value" << parameter.value;
            storage << "}";
        }
        storage << "]";

        storage << "numeric_artifacts" << "[";
        for (const auto& artifact : record.numeric_artifacts) {
            storage << "{";
            storage << "name" << artifact.name;
            storage << "matrix" << artifact.value;
            storage << "}";
        }
        storage << "]";
    } catch (const cv::Exception& exception) {
        throw std::runtime_error(
            "OpenCV could not write processing YAML file "
            + output_path.string() + ": " + exception.what()
        );
    }
}

ProcessingRecord ProcessingDataStorage::load_yaml(
    const std::filesystem::path& input_path
) const {
    require_yaml_extension(input_path);

    try {
        cv::FileStorage storage(input_path.string(), cv::FileStorage::READ);

        if (!storage.isOpened()) {
            throw std::runtime_error(
                "Could not open processing data file for reading: "
                + input_path.string()
            );
        }

        ProcessingRecord record{
            .format_version =
                read_required_string(storage, "format_version"),
            .project_version =
                read_required_string(storage, "project_version"),
            .laboratory = read_required_string(storage, "laboratory"),
            .operation = read_required_string(storage, "operation"),
            .input_path = read_required_string(storage, "input_path"),
            .parameters = {},
            .numeric_artifacts = {},
        };

        const cv::FileNode parameters = storage["parameters"];
        if (parameters.empty() || !parameters.isSeq()) {
            throw std::runtime_error(
                "Required YAML sequence is missing or invalid: parameters"
            );
        }

        for (const auto& node : parameters) {
            const cv::FileNode name_node = node["name"];
            const cv::FileNode value_node = node["value"];

            if (name_node.empty() || !name_node.isString()
                || value_node.empty() || !value_node.isString()) {
                throw std::runtime_error(
                    "Invalid parameter entry in processing YAML."
                );
            }

            record.parameters.push_back({
                static_cast<std::string>(name_node),
                static_cast<std::string>(value_node),
            });
        }

        const cv::FileNode artifacts = storage["numeric_artifacts"];
        if (artifacts.empty() || !artifacts.isSeq()) {
            throw std::runtime_error(
                "Required YAML sequence is missing or invalid: "
                "numeric_artifacts"
            );
        }

        for (const auto& node : artifacts) {
            const cv::FileNode name_node = node["name"];
            const cv::FileNode matrix_node = node["matrix"];

            if (name_node.empty() || !name_node.isString()
                || matrix_node.empty()) {
                throw std::runtime_error(
                    "Invalid numeric artifact entry in processing YAML."
                );
            }

            cv::Mat matrix;
            matrix_node >> matrix;

            if (matrix.empty()) {
                throw std::runtime_error(
                    "Numeric artifact matrix is empty in processing YAML."
                );
            }

            record.numeric_artifacts.push_back({
                static_cast<std::string>(name_node),
                matrix.clone(),
            });
        }

        validate_record(record);
        return record;
    } catch (const cv::Exception& exception) {
        throw std::runtime_error(
            "OpenCV could not read processing YAML file "
            + input_path.string() + ": " + exception.what()
        );
    }
}

} // namespace pdi::io

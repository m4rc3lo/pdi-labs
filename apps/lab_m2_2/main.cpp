/**
 * @file main.cpp
 * @brief Integrates connected-component labeling outputs for Laboratory M2.2.
 */

#include "pdi/io/component_features_csv_storage.hpp"
#include "pdi/io/processing_data_storage.hpp"
#include "pdi/segmentation/component_feature_extractor.hpp"
#include "pdi/segmentation/connected_component_labeler.hpp"
#include "pdi/segmentation/label_visualizer.hpp"

#include <opencv2/imgcodecs.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    pdi::segmentation::Connectivity connectivity =
        pdi::segmentation::Connectivity::Eight;
    bool save_data = false;
    bool validate_opencv = false;
};

[[nodiscard]] Options parse_options(
    const int argc,
    char** argv
) {
    if (argc < 3) {
        throw std::invalid_argument(
            "Usage: lab_m2_2 <input-image> <output-directory> "
            "[--connectivity 4|8] [--save-data] [--validate-opencv]"
        );
    }

    Options options{
        .input_path = argv[1],
        .output_directory = argv[2],
    };

    for (int index = 3; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--save-data") {
            options.save_data = true;
            continue;
        }

        if (argument == "--validate-opencv") {
            options.validate_opencv = true;
            continue;
        }

        if (argument == "--connectivity") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value for --connectivity."
                );
            }

            const std::string value = argv[++index];

            if (value == "4") {
                options.connectivity =
                    pdi::segmentation::Connectivity::Four;
                continue;
            }

            if (value == "8") {
                options.connectivity =
                    pdi::segmentation::Connectivity::Eight;
                continue;
            }

            throw std::invalid_argument(
                "Connectivity must be 4 or 8."
            );
        }

        throw std::invalid_argument("Unknown option: " + argument);
    }

    return options;
}

[[nodiscard]] std::string connectivity_to_string(
    const pdi::segmentation::Connectivity connectivity
) {
    return connectivity == pdi::segmentation::Connectivity::Four ? "4" : "8";
}

} // namespace

int main(
    const int argc,
    char** argv
) {
    try {
        const Options options = parse_options(argc, argv);

        std::error_code error;
        std::filesystem::create_directories(options.output_directory, error);

        if (error) {
            throw std::runtime_error(
                "Could not create output directory: "
                + options.output_directory.string()
                + ": " + error.message()
            );
        }

        const cv::Mat input_image = cv::imread(
            options.input_path.string(),
            cv::IMREAD_GRAYSCALE
        );

        if (input_image.empty()) {
            throw std::runtime_error(
                "Could not read input image: " + options.input_path.string()
            );
        }

        const pdi::segmentation::ConnectedComponentLabelingResult labeling =
            pdi::segmentation::ConnectedComponentLabeler{}.label(
                input_image,
                options.connectivity
            );

        const pdi::segmentation::LabelVisualizer visualizer;
        const cv::Mat labels_image = visualizer.colorize(labeling.labels);
        const auto features =
            pdi::segmentation::ComponentFeatureExtractor{}.extract(
                labeling.labels
            );

        const std::filesystem::path labels_path =
            options.output_directory / "labels.png";
        const std::filesystem::path csv_path =
            options.output_directory / "components.csv";

        if (!cv::imwrite(labels_path.string(), labels_image)) {
            throw std::runtime_error(
                "Could not write labels image: " + labels_path.string()
            );
        }

        pdi::io::ComponentFeaturesCsvStorage{}.save(csv_path, features);

        auto record =
            pdi::segmentation::ComponentFeatureExtractor{}
                .make_processing_record(
                    labeling.labels,
                    features,
                    {
                        .project_version = "0.5.0",
                        .laboratory = "M2.2",
                        .operation = "connected_components",
                        .input_path = options.input_path.string(),
                        .connectivity = connectivity_to_string(
                            options.connectivity
                        ),
                    }
                );

        if (options.validate_opencv) {
            const pdi::segmentation::LabelValidationSummary validation =
                visualizer.compare_with_opencv(
                    input_image,
                    labeling.labels,
                    options.connectivity
                );

            record.parameters.push_back({
                "validation.enabled",
                "true",
            });
            record.parameters.push_back({
                "validation.manual_component_count",
                std::to_string(validation.manual_component_count),
            });
            record.parameters.push_back({
                "validation.opencv_component_count",
                std::to_string(validation.opencv_component_count),
            });
            record.parameters.push_back({
                "validation.same_partition",
                validation.same_partition ? "true" : "false",
            });

            std::cout
                << "OpenCV validation: manual="
                << validation.manual_component_count
                << ", opencv="
                << validation.opencv_component_count
                << ", same_partition="
                << (validation.same_partition ? "true" : "false")
                << '\n';
        } else {
            record.parameters.push_back({
                "validation.enabled",
                "false",
            });
        }

        if (options.save_data) {
            const std::filesystem::path yaml_path =
                options.output_directory / "components_result.yml";
            pdi::io::ProcessingDataStorage{}.save_yaml(yaml_path, record);
        }

        std::cout
            << "Generated: " << labels_path.string() << '\n'
            << "Generated: " << csv_path.string() << '\n';

        if (options.save_data) {
            std::cout
                << "Generated: "
                << (options.output_directory / "components_result.yml")
                       .string()
                << '\n';
        }
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}

/**
 * @file main.cpp
 * @brief Runs documented OpenCV-based segmentation examples.
 */

#include "pdi/io/image_file_storage.hpp"
#include "pdi/io/processing_data_storage.hpp"
#include "pdi/segmentation/library_segmentation_pipeline.hpp"
#include "pdi/version.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

using Operation = pdi::segmentation::LibrarySegmentationOperation;

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    Operation operation;
    int block_size = 11;
    double constant_c = 2.0;
    int distance_mask_size = 3;
    double foreground_ratio = 0.4;
    bool save_data = false;
};

Operation parse_operation(const std::string& value) {
    if (value == "hsv") return Operation::BgrToHsv;
    if (value == "otsu") return Operation::Otsu;
    if (value == "adaptive") return Operation::AdaptiveMean;
    if (value == "distance") return Operation::DistanceTransform;
    if (value == "watershed") return Operation::Watershed;
    throw std::invalid_argument("Unsupported operation: " + value);
}

Options parse_options(int argc, char* argv[]) {
    if (argc < 4) {
        throw std::invalid_argument(
            "Usage: lab_m2_1_library <input> <output-directory> "
            "<hsv|otsu|adaptive|distance|watershed> "
            "[--block-size N] [--constant C] [--distance-mask 3|5] "
            "[--foreground-ratio R] [--save-data]"
        );
    }

    Options options{
        .input_path = argv[1],
        .output_directory = argv[2],
        .operation = parse_operation(argv[3]),
    };

    for (int index = 4; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--save-data") {
            options.save_data = true;
        } else if (argument == "--block-size" && index + 1 < argc) {
            options.block_size = std::stoi(argv[++index]);
        } else if (argument == "--constant" && index + 1 < argc) {
            options.constant_c = std::stod(argv[++index]);
        } else if (argument == "--distance-mask" && index + 1 < argc) {
            options.distance_mask_size = std::stoi(argv[++index]);
        } else if (argument == "--foreground-ratio" && index + 1 < argc) {
            options.foreground_ratio = std::stod(argv[++index]);
        } else {
            throw std::invalid_argument(
                "Unsupported or incomplete argument: " + argument
            );
        }
    }

    return options;
}

bool requires_color(Operation operation) {
    return operation == Operation::BgrToHsv
        || operation == Operation::Watershed;
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const Options options = parse_options(argc, argv);
        const pdi::io::ImageFileStorage storage;

        const cv::Mat input = requires_color(options.operation)
            ? storage.load_color(options.input_path)
            : storage.load_grayscale(options.input_path);

        const auto result =
            pdi::segmentation::LibrarySegmentationPipeline{}.run(
                input,
                {
                    .operation = options.operation,
                    .adaptive_block_size = options.block_size,
                    .adaptive_constant = options.constant_c,
                    .distance_mask_size =
                        static_cast<double>(options.distance_mask_size),
                    .foreground_ratio = options.foreground_ratio,
                }
            );

        for (const auto& output : result.visual_outputs) {
            storage.save(
                options.output_directory / (output.name + ".png"),
                output.image
            );
        }

        if (options.save_data) {
            pdi::io::ProcessingDataStorage{}.save_yaml(
                options.output_directory / result.data_file_name,
                {
                    .format_version = "1",
                    .project_version = PDI_PROJECT_VERSION,
                    .laboratory = "M2.1",
                    .operation = result.operation_name,
                    .input_path = options.input_path.string(),
                    .parameters = result.parameters,
                    .numeric_artifacts = result.numeric_artifacts,
                }
            );
        }

        std::cout
            << "Operation: " << result.operation_name << '\n'
            << "Visual outputs: " << result.visual_outputs.size() << '\n'
            << "Numeric artifacts: "
            << result.numeric_artifacts.size() << '\n';
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

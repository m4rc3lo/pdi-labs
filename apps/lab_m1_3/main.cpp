/**
 * @file main.cpp
 * @brief Integrated command-line application for Laboratory M1.3.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/labs/m1_3_pipeline.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    pdi::labs::M13Operation operation;
    pdi::spatial::BorderStrategy border_strategy;
    double enhancement_factor;
    bool show_images;
};

[[nodiscard]] pdi::labs::M13Operation parse_operation(
    const std::string& value
) {
    if (value == "mean3") {
        return pdi::labs::M13Operation::Mean3x3;
    }
    if (value == "weighted3") {
        return pdi::labs::M13Operation::WeightedMean3x3;
    }
    if (value == "mean5") {
        return pdi::labs::M13Operation::Mean5x5;
    }
    if (value == "laplacian4") {
        return pdi::labs::M13Operation::Laplacian4;
    }
    if (value == "laplacian8") {
        return pdi::labs::M13Operation::Laplacian8;
    }
    if (value == "sobel") {
        return pdi::labs::M13Operation::Sobel;
    }

    throw std::invalid_argument("Unsupported operation: " + value);
}

[[nodiscard]] pdi::spatial::BorderStrategy parse_border(
    const std::string& value
) {
    if (value == "copy") {
        return pdi::spatial::BorderStrategy::CopyBorder;
    }
    if (value == "replicate") {
        return pdi::spatial::BorderStrategy::ReplicateBorder;
    }

    throw std::invalid_argument("Unsupported border strategy: " + value);
}

[[nodiscard]] Options parse_options(int argc, char* argv[]) {
    if (argc < 5) {
        throw std::invalid_argument(
            "Usage: lab_m1_3 <input-image> <output-directory> "
            "--operation <mean3|weighted3|mean5|laplacian4|"
            "laplacian8|sobel> [--border <copy|replicate>] "
            "[--factor <value>] [--show]"
        );
    }

    Options options{
        .input_path = argv[1],
        .output_directory = argv[2],
        .operation = pdi::labs::M13Operation::Mean3x3,
        .border_strategy =
            pdi::spatial::BorderStrategy::ReplicateBorder,
        .enhancement_factor = 1.0,
        .show_images = false,
    };

    bool operation_defined = false;

    for (int index = 3; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--operation") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --operation."
                );
            }
            options.operation = parse_operation(argv[++index]);
            operation_defined = true;
        } else if (argument == "--border") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --border."
                );
            }
            options.border_strategy = parse_border(argv[++index]);
        } else if (argument == "--factor") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --factor."
                );
            }
            options.enhancement_factor = std::stod(argv[++index]);
        } else if (argument == "--show") {
            options.show_images = true;
        } else {
            throw std::invalid_argument(
                "Unsupported argument: " + argument
            );
        }
    }

    if (!operation_defined) {
        throw std::invalid_argument("--operation is required.");
    }

    return options;
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const Options options = parse_options(argc, argv);
        const pdi::io::ImageFileStorage storage;
        const cv::Mat input = storage.load_grayscale(options.input_path);

        const auto outputs = pdi::labs::M13Pipeline{}.run(
            input,
            {
                .operation = options.operation,
                .border_strategy = options.border_strategy,
                .enhancement_factor = options.enhancement_factor,
            }
        );

        std::vector<pdi::io::WindowImage> windows{
            {"Input grayscale", input},
        };

        for (const auto& output : outputs) {
            const auto output_path =
                options.output_directory / (output.name + ".png");
            storage.save(output_path, output.image);
            windows.push_back({output.name, output.image});
            std::cout << "Saved: " << output_path.string() << '\n';
        }

        if (options.show_images) {
            pdi::io::ImageDisplay{}.show_all(windows);
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

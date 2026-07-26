/**
 * @file main.cpp
 * @brief Reproducible Laplacian enhancement example for M1.3.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/spatial/laplacian_enhancement.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    double factor;
    bool show_images;
};

[[nodiscard]] Options parse_options(int argc, char* argv[]) {
    if (argc != 4 && argc != 5) {
        throw std::invalid_argument(
            "Usage: lab_m1_3_laplacian <input-image> "
            "<output-directory> <factor> [--show]"
        );
    }

    const bool show_images =
        argc == 5 && std::string{argv[4]} == "--show";

    if (argc == 5 && !show_images) {
        throw std::invalid_argument(
            "Usage: lab_m1_3_laplacian <input-image> "
            "<output-directory> <factor> [--show]"
        );
    }

    return {
        .input_path = argv[1],
        .output_directory = argv[2],
        .factor = std::stod(argv[3]),
        .show_images = show_images,
    };
}

void save_result(
    const pdi::io::ImageFileStorage& storage,
    const std::filesystem::path& directory,
    const std::string& prefix,
    const pdi::spatial::LaplacianEnhancementResult& result
) {
    storage.save(
        directory / (prefix + "_response_visualization.png"),
        result.response_visualization
    );
    storage.save(
        directory / (prefix + "_enhanced.png"),
        result.enhanced_image
    );
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const Options options = parse_options(argc, argv);
        const pdi::io::ImageFileStorage storage;
        const cv::Mat input = storage.load_grayscale(options.input_path);
        const pdi::spatial::LaplacianEnhancement enhancement;

        const auto four = enhancement.apply(
            input,
            pdi::spatial::LaplacianKernel::FourNeighbor,
            options.factor
        );
        const auto eight = enhancement.apply(
            input,
            pdi::spatial::LaplacianKernel::EightNeighbor,
            options.factor
        );

        save_result(storage, options.output_directory, "laplacian_4", four);
        save_result(storage, options.output_directory, "laplacian_8", eight);

        std::cout
            << "M1.3 Laplacian enhancement completed.\n"
            << "Enhancement factor: " << options.factor << '\n'
            << "Raw responses remain CV_64FC1 in memory.\n"
            << "Saved response images are min-max normalized views only.\n";

        if (options.show_images) {
            pdi::io::ImageDisplay{}.show_all({
                {"Input grayscale", input},
                {"Laplacian 4 response", four.response_visualization},
                {"Laplacian 4 enhanced", four.enhanced_image},
                {"Laplacian 8 response", eight.response_visualization},
                {"Laplacian 8 enhanced", eight.enhanced_image},
            });
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

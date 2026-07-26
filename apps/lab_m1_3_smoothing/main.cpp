/**
 * @file main.cpp
 * @brief Reproducible example of M1.3 smoothing filters.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/spatial/smoothing_filters.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    bool show_images;
};

[[nodiscard]] Options parse_options(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        throw std::invalid_argument(
            "Usage: lab_m1_3_smoothing <input-image> "
            "<output-directory> [--show]"
        );
    }

    const bool show_images =
        argc == 4 && std::string{argv[3]} == "--show";

    if (argc == 4 && !show_images) {
        throw std::invalid_argument(
            "Usage: lab_m1_3_smoothing <input-image> "
            "<output-directory> [--show]"
        );
    }

    return {
        .input_path = argv[1],
        .output_directory = argv[2],
        .show_images = show_images,
    };
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const Options options = parse_options(argc, argv);
        const pdi::io::ImageFileStorage storage;
        const cv::Mat input = storage.load_grayscale(options.input_path);

        const pdi::spatial::SmoothingFilters filters;
        const auto border =
            pdi::spatial::BorderStrategy::ReplicateBorder;

        const cv::Mat mean_3x3 = filters.mean_3x3(input, border);
        const cv::Mat weighted_3x3 =
            filters.weighted_mean_3x3(input, border);
        const cv::Mat mean_5x5 = filters.mean_5x5(input, border);

        storage.save(
            options.output_directory / "mean_3x3.png",
            mean_3x3
        );
        storage.save(
            options.output_directory / "weighted_mean_3x3.png",
            weighted_3x3
        );
        storage.save(
            options.output_directory / "mean_5x5.png",
            mean_5x5
        );

        std::cout
            << "M1.3 smoothing example completed.\n"
            << "Border strategy: ReplicateBorder\n"
            << "Output directory: "
            << options.output_directory.string()
            << '\n';

        if (options.show_images) {
            pdi::io::ImageDisplay{}.show_all({
                {"Input grayscale", input},
                {"Mean 3x3", mean_3x3},
                {"Weighted mean 3x3", weighted_3x3},
                {"Mean 5x5", mean_5x5},
            });
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

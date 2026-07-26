/**
 * @file main.cpp
 * @brief Reproducible manual Sobel example for M1.3.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/spatial/sobel_operator.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    bool show_images;
};

[[nodiscard]] Options parse_options(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        throw std::invalid_argument(
            "Usage: lab_m1_3_sobel <input-image> "
            "<output-directory> [--show]"
        );
    }

    const bool show_images =
        argc == 4 && std::string{argv[3]} == "--show";

    if (argc == 4 && !show_images) {
        throw std::invalid_argument(
            "Usage: lab_m1_3_sobel <input-image> "
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

        const auto result = pdi::spatial::SobelOperator{}.apply(input);

        storage.save(
            options.output_directory / "sobel_gx_visualization.png",
            result.gradient_x_visualization
        );
        storage.save(
            options.output_directory / "sobel_gy_visualization.png",
            result.gradient_y_visualization
        );
        storage.save(
            options.output_directory / "sobel_magnitude_approximate.png",
            result.magnitude_approximate_visualization
        );
        storage.save(
            options.output_directory / "sobel_magnitude_euclidean.png",
            result.magnitude_euclidean_visualization
        );

        std::cout
            << "M1.3 Sobel completed.\n"
            << "Signed Gx and Gy remain CV_64FC1 in memory.\n"
            << "Saved images are normalized visualizations.\n";

        if (options.show_images) {
            pdi::io::ImageDisplay{}.show_all({
                {"Input grayscale", input},
                {"Sobel Gx", result.gradient_x_visualization},
                {"Sobel Gy", result.gradient_y_visualization},
                {
                    "Sobel magnitude approximate",
                    result.magnitude_approximate_visualization
                },
                {
                    "Sobel magnitude Euclidean",
                    result.magnitude_euclidean_visualization
                },
            });
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

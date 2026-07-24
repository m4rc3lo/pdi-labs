/**
 * @file main.cpp
 * @brief Generates, saves, and optionally displays grayscale conversions.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/value/grayscale_converter.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

[[nodiscard]] bool parse_show_option(int argc, char* argv[]) {
    if (argc == 3) {
        return false;
    }

    if (argc == 4 && std::string{argv[3]} == "--show") {
        return true;
    }

    throw std::invalid_argument(
        "Usage: lab_m1_1_grayscale <input-image> <output-directory> [--show]"
    );
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const bool show_images = parse_show_option(argc, argv);
        const std::filesystem::path input_path = argv[1];
        const std::filesystem::path output_directory = argv[2];

        const pdi::io::ImageFileStorage storage;
        const cv::Mat input_image = storage.load_color(input_path);

        const pdi::value::GrayscaleConverter converter;
        const cv::Mat average = converter.convert_average(input_image);
        const cv::Mat weighted = converter.convert_weighted(input_image);

        storage.save(output_directory / "grayscale_average.png", average);
        storage.save(output_directory / "grayscale_weighted.png", weighted);

        std::cout
            << "Saved:\n"
            << "  grayscale_average.png\n"
            << "  grayscale_weighted.png\n";

        if (show_images) {
            pdi::io::ImageDisplay{}.show_all({
                {"M1.1 Grayscale - Input BGR", input_image},
                {"M1.1 Grayscale - Arithmetic mean", average},
                {"M1.1 Grayscale - Weighted luminance", weighted},
            });
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

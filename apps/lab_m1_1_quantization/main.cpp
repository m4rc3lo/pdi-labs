/**
 * @file main.cpp
 * @brief Generates, saves, and optionally displays grayscale quantizations.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/value/grayscale_quantizer.hpp"

#include <array>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct CommandLineOptions {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    bool show_images;
};

[[nodiscard]] CommandLineOptions parse_options(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        throw std::invalid_argument(
            "Usage: lab_m1_1_quantization "
            "<input-image> <output-directory> [--show]"
        );
    }

    const bool show_images =
        argc == 4 && std::string{argv[3]} == "--show";

    if (argc == 4 && !show_images) {
        throw std::invalid_argument(
            "Usage: lab_m1_1_quantization "
            "<input-image> <output-directory> [--show]"
        );
    }

    return {
        .input_path = argv[1],
        .output_directory = argv[2],
        .show_images = show_images,
    };
}

[[nodiscard]] std::string output_filename(int levels) {
    std::ostringstream filename;
    filename
        << "quantized_"
        << std::setw(3)
        << std::setfill('0')
        << levels
        << "_levels.png";

    return filename.str();
}

[[nodiscard]] std::string window_title(int levels) {
    return "M1.1 Quantization - "
        + std::to_string(levels)
        + " levels";
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const CommandLineOptions options = parse_options(argc, argv);
        const pdi::io::ImageFileStorage storage;
        const cv::Mat input_image =
            storage.load_grayscale(options.input_path);

        constexpr std::array<int, 5> level_counts{2, 4, 8, 16, 256};

        const pdi::value::GrayscaleQuantizer quantizer;
        std::vector<pdi::io::WindowImage> windows;

        if (options.show_images) {
            windows.push_back({
                "M1.1 Quantization - Input grayscale",
                input_image,
            });
        }

        std::cout << "Saved:\n";

        for (const int levels : level_counts) {
            const cv::Mat result = quantizer.quantize(input_image, levels);
            const std::string filename = output_filename(levels);

            storage.save(options.output_directory / filename, result);
            std::cout << "  " << filename << '\n';

            if (options.show_images) {
                windows.push_back({window_title(levels), result});
            }
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

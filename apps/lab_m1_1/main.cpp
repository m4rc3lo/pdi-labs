/**
 * @file main.cpp
 * @brief Runs the complete Laboratory M1.1 workflow.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/labs/m1_1_pipeline.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct CommandLineOptions {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    bool show_images;
};

[[nodiscard]] CommandLineOptions parse_options(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        throw std::invalid_argument(
            "Usage: lab_m1_1 <input-image> <output-directory> [--show]"
        );
    }

    const bool show_images =
        argc == 4 && std::string{argv[3]} == "--show";

    if (argc == 4 && !show_images) {
        throw std::invalid_argument(
            "Usage: lab_m1_1 <input-image> <output-directory> [--show]"
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
        const CommandLineOptions options = parse_options(argc, argv);
        const pdi::labs::M11Pipeline pipeline;

        const pdi::labs::M11Results results =
            pipeline.run(options.input_path, options.output_directory);

        std::cout
            << "Laboratory M1.1 completed.\n"
            << "Output directory: "
            << options.output_directory.string()
            << '\n';

        if (options.show_images) {
            pdi::io::ImageDisplay{}.show_all(
                pipeline.display_images(results)
            );
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

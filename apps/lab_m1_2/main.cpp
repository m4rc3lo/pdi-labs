/**
 * @file main.cpp
 * @brief Runs the preliminary applied study for Laboratory M1.2.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/labs/m1_2_study_case.hpp"

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
            "Usage: lab_m1_2 <input-image> <output-directory> [--show]"
        );
    }

    const bool show_images =
        argc == 4 && std::string{argv[3]} == "--show";

    if (argc == 4 && !show_images) {
        throw std::invalid_argument(
            "Usage: lab_m1_2 <input-image> <output-directory> [--show]"
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
        const pdi::labs::M12StudyCase study;

        const pdi::labs::M12StudyResult result =
            study.run(options.input_path, options.output_directory);

        std::cout
            << "Laboratory M1.2 preliminary study completed.\n"
            << "Distinct levels in four-level result: "
            << result.distinct_quantized_levels
            << '\n'
            << "Preliminary success: "
            << (result.success ? "yes" : "no")
            << '\n'
            << "Output directory: "
            << options.output_directory.string()
            << '\n';

        if (options.show_images) {
            pdi::io::ImageDisplay{}.show_all(
                study.display_images(result)
            );
        }

        return result.success ? 0 : 2;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

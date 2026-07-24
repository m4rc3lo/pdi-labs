/**
 * @file main.cpp
 * @brief Runs manual BGR channel separation, persistence, and optional display.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/value/channel_separator.hpp"

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
        "Usage: lab_m1_1_channels <input-image> <output-directory> [--show]"
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

        const pdi::value::ChannelSet channels =
            pdi::value::ChannelSeparator{}.separate(input_image);

        storage.save(output_directory / "channel_blue.png", channels.blue);
        storage.save(output_directory / "channel_green.png", channels.green);
        storage.save(output_directory / "channel_red.png", channels.red);

        std::cout << "Saved:\n"
                  << "  channel_blue.png\n"
                  << "  channel_green.png\n"
                  << "  channel_red.png\n";

        if (show_images) {
            pdi::io::ImageDisplay{}.show_all({
                {"M1.1 Channels - Input BGR", input_image},
                {"M1.1 Channels - Blue intensity", channels.blue},
                {"M1.1 Channels - Green intensity", channels.green},
                {"M1.1 Channels - Red intensity", channels.red},
            });
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

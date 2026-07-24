/**
 * @file main.cpp
 * @brief Runs manual BGR channel separation and saves the results.
 */

#include "pdi/value/channel_separator.hpp"

#include <opencv2/imgcodecs.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void write_image(const std::filesystem::path& path, const cv::Mat& image) {
    if (!cv::imwrite(path.string(), image)) {
        throw std::runtime_error("Could not write output image: " + path.string());
    }
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr
                << "Usage: lab_m1_1_channels <input-image> <output-directory>\n";
            return 1;
        }

        const std::filesystem::path input_path = argv[1];
        const std::filesystem::path output_directory = argv[2];

        const cv::Mat input_image = cv::imread(input_path.string(), cv::IMREAD_COLOR);

        if (input_image.empty()) {
            throw std::runtime_error(
                "Could not read input image: " + input_path.string()
            );
        }

        std::filesystem::create_directories(output_directory);

        const pdi::value::ChannelSet channels =
            pdi::value::ChannelSeparator{}.separate(input_image);

        write_image(output_directory / "channel_blue.png", channels.blue);
        write_image(output_directory / "channel_green.png", channels.green);
        write_image(output_directory / "channel_red.png", channels.red);

        std::cout << "Saved:\n"
                  << "  channel_blue.png\n"
                  << "  channel_green.png\n"
                  << "  channel_red.png\n";

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

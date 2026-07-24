/**
 * @file main.cpp
 * @brief Generates manual grayscale conversions from one BGR image.
 */

#include "pdi/value/grayscale_converter.hpp"

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
                << "Usage: lab_m1_1_grayscale <input-image> <output-directory>\n";
            return 1;
        }

        const std::filesystem::path input_path = argv[1];
        const std::filesystem::path output_directory = argv[2];

        const cv::Mat input_image =
            cv::imread(input_path.string(), cv::IMREAD_COLOR);

        if (input_image.empty()) {
            throw std::runtime_error(
                "Could not read input image: " + input_path.string()
            );
        }

        std::filesystem::create_directories(output_directory);

        const pdi::value::GrayscaleConverter converter;
        const cv::Mat average = converter.convert_average(input_image);
        const cv::Mat weighted = converter.convert_weighted(input_image);

        write_image(output_directory / "grayscale_average.png", average);
        write_image(output_directory / "grayscale_weighted.png", weighted);

        std::cout
            << "Saved:\n"
            << "  grayscale_average.png\n"
            << "  grayscale_weighted.png\n";

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

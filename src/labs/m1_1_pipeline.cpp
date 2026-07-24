/**
 * @file m1_1_pipeline.cpp
 * @brief Implements the integrated Laboratory M1.1 pipeline.
 */

#include "pdi/labs/m1_1_pipeline.hpp"

#include "pdi/io/image_file_storage.hpp"
#include "pdi/value/channel_separator.hpp"
#include "pdi/value/grayscale_converter.hpp"
#include "pdi/value/grayscale_quantizer.hpp"
#include "pdi/value/image_copy.hpp"
#include "pdi/value/image_inspector.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void write_intensity_csv(
    std::ofstream& report,
    const std::string& scope,
    const pdi::value::IntensityStatistics& statistics
) {
    report
        << scope << ','
        << static_cast<int>(statistics.minimum) << ','
        << static_cast<int>(statistics.maximum) << ','
        << statistics.sum << ','
        << std::fixed << std::setprecision(6) << statistics.mean
        << '\n';
}

void write_report(
    const std::filesystem::path& report_path,
    const pdi::value::ImageStatistics& statistics
) {
    std::ofstream report(report_path);

    if (!report) {
        throw std::runtime_error(
            "Could not create inspection report: " + report_path.string()
        );
    }

    report << "property,value\n";
    report << "width," << statistics.width << '\n';
    report << "height," << statistics.height << '\n';
    report << "pixel_count," << statistics.pixel_count << '\n';
    report << "channel_count," << statistics.channel_count << '\n';
    report << "opencv_type," << statistics.opencv_type << '\n';
    report << '\n';
    report << "scope,minimum,maximum,sum,mean\n";

    write_intensity_csv(report, "global", statistics.intensity);

    if (statistics.bgr.has_value()) {
        write_intensity_csv(report, "blue", statistics.bgr->blue);
        write_intensity_csv(report, "green", statistics.bgr->green);
        write_intensity_csv(report, "red", statistics.bgr->red);
    }

    if (!report) {
        throw std::runtime_error(
            "Could not finish inspection report: " + report_path.string()
        );
    }
}

} // namespace

namespace pdi::labs {

M11Results M11Pipeline::run(
    const std::filesystem::path& input_path,
    const std::filesystem::path& output_directory
) const {
    const pdi::io::ImageFileStorage storage;
    const cv::Mat input = storage.load_color(input_path);

    const pdi::value::ImageStatistics statistics =
        pdi::value::ImageInspector{}.inspect(input);

    const cv::Mat manual_copy =
        pdi::value::ImageCopy{}.copy(input);

    const pdi::value::ChannelSet channels =
        pdi::value::ChannelSeparator{}.separate(input);

    const pdi::value::GrayscaleConverter converter;
    const cv::Mat grayscale_average =
        converter.convert_average(input);
    const cv::Mat grayscale_weighted =
        converter.convert_weighted(input);

    const pdi::value::GrayscaleQuantizer quantizer;
    const cv::Mat quantized_16 =
        quantizer.quantize(grayscale_weighted, 16);
    const cv::Mat quantized_8 =
        quantizer.quantize(grayscale_weighted, 8);
    const cv::Mat quantized_4 =
        quantizer.quantize(grayscale_weighted, 4);
    const cv::Mat quantized_2 =
        quantizer.quantize(grayscale_weighted, 2);

    storage.save(output_directory / "manual_copy.png", manual_copy);
    storage.save(output_directory / "channel_blue.png", channels.blue);
    storage.save(output_directory / "channel_green.png", channels.green);
    storage.save(output_directory / "channel_red.png", channels.red);
    storage.save(
        output_directory / "grayscale_average.png",
        grayscale_average
    );
    storage.save(
        output_directory / "grayscale_weighted.png",
        grayscale_weighted
    );
    storage.save(
        output_directory / "quantized_016_levels.png",
        quantized_16
    );
    storage.save(
        output_directory / "quantized_008_levels.png",
        quantized_8
    );
    storage.save(
        output_directory / "quantized_004_levels.png",
        quantized_4
    );
    storage.save(
        output_directory / "quantized_002_levels.png",
        quantized_2
    );

    write_report(
        output_directory / "inspection_report.csv",
        statistics
    );

    return {
        .input = input,
        .manual_copy = manual_copy,
        .blue_channel = channels.blue,
        .green_channel = channels.green,
        .red_channel = channels.red,
        .grayscale_average = grayscale_average,
        .grayscale_weighted = grayscale_weighted,
        .quantized_16 = quantized_16,
        .quantized_8 = quantized_8,
        .quantized_4 = quantized_4,
        .quantized_2 = quantized_2,
    };
}

std::vector<pdi::io::WindowImage> M11Pipeline::display_images(
    const M11Results& results
) const {
    return {
        {"M1.1 - Input BGR", results.input},
        {"M1.1 - Manual copy", results.manual_copy},
        {"M1.1 - Blue channel", results.blue_channel},
        {"M1.1 - Green channel", results.green_channel},
        {"M1.1 - Red channel", results.red_channel},
        {"M1.1 - Grayscale arithmetic mean", results.grayscale_average},
        {"M1.1 - Grayscale weighted", results.grayscale_weighted},
        {"M1.1 - Quantized 16 levels", results.quantized_16},
        {"M1.1 - Quantized 8 levels", results.quantized_8},
        {"M1.1 - Quantized 4 levels", results.quantized_4},
        {"M1.1 - Quantized 2 levels", results.quantized_2},
    };
}

} // namespace pdi::labs

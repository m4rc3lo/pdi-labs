/**
 * @file m1_2_study_case.cpp
 * @brief Implements the preliminary applied study for Laboratory M1.2.
 */

#include "pdi/labs/m1_2_study_case.hpp"

#include "pdi/io/image_file_storage.hpp"
#include "pdi/value/grayscale_converter.hpp"
#include "pdi/value/grayscale_quantizer.hpp"
#include "pdi/value/image_inspector.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

[[nodiscard]] int count_distinct_levels(const cv::Mat& image) {
    std::array<bool, 256> present{};
    int count = 0;

    for (int row = 0; row < image.rows; ++row) {
        const auto* row_ptr = image.ptr<std::uint8_t>(row);

        for (int col = 0; col < image.cols; ++col) {
            const std::uint8_t value = row_ptr[col];

            if (!present[value]) {
                present[value] = true;
                ++count;
            }
        }
    }

    return count;
}

void write_report(
    const std::filesystem::path& report_path,
    const pdi::value::ImageStatistics& input_statistics,
    const pdi::value::ImageStatistics& grayscale_statistics,
    int distinct_levels,
    bool success
) {
    std::ofstream report(report_path);

    if (!report) {
        throw std::runtime_error(
            "Could not create M1.2 study report: " + report_path.string()
        );
    }

    report << "property,value\n";
    report << "problem,preliminary illumination-zone simplification\n";
    report << "input_width," << input_statistics.width << '\n';
    report << "input_height," << input_statistics.height << '\n';
    report << "input_channels," << input_statistics.channel_count << '\n';
    report << "weighted_grayscale_minimum,"
           << static_cast<int>(grayscale_statistics.intensity.minimum)
           << '\n';
    report << "weighted_grayscale_maximum,"
           << static_cast<int>(grayscale_statistics.intensity.maximum)
           << '\n';
    report << "quantized_4_distinct_levels," << distinct_levels << '\n';
    report << "success_threshold,3\n";
    report << "preliminary_success," << (success ? "true" : "false") << '\n';

    if (!report) {
        throw std::runtime_error(
            "Could not finish M1.2 study report: " + report_path.string()
        );
    }
}

} // namespace

namespace pdi::labs {

M12StudyResult M12StudyCase::run(
    const std::filesystem::path& input_path,
    const std::filesystem::path& output_directory
) const {
    const pdi::io::ImageFileStorage storage;
    const cv::Mat input = storage.load_color(input_path);

    const cv::Mat grayscale_weighted =
        pdi::value::GrayscaleConverter{}.convert_weighted(input);

    const pdi::value::GrayscaleQuantizer quantizer;
    const cv::Mat quantized_8 = quantizer.quantize(grayscale_weighted, 8);
    const cv::Mat quantized_4 = quantizer.quantize(grayscale_weighted, 4);

    const int distinct_levels = count_distinct_levels(quantized_4);
    const bool success = distinct_levels >= 3;

    storage.save(
        output_directory / "grayscale_weighted.png",
        grayscale_weighted
    );
    storage.save(
        output_directory / "quantized_008_levels.png",
        quantized_8
    );
    storage.save(
        output_directory / "quantized_004_levels.png",
        quantized_4
    );

    const pdi::value::ImageInspector inspector;
    write_report(
        output_directory / "study_report.csv",
        inspector.inspect(input),
        inspector.inspect(grayscale_weighted),
        distinct_levels,
        success
    );

    return {
        .input = input,
        .grayscale_weighted = grayscale_weighted,
        .quantized_8 = quantized_8,
        .quantized_4 = quantized_4,
        .distinct_quantized_levels = distinct_levels,
        .success = success,
    };
}

std::vector<pdi::io::WindowImage> M12StudyCase::display_images(
    const M12StudyResult& result
) const {
    return {
        {"M1.2 Study - Input BGR", result.input},
        {"M1.2 Study - Weighted grayscale", result.grayscale_weighted},
        {"M1.2 Study - Quantized 8 levels", result.quantized_8},
        {"M1.2 Study - Quantized 4 levels", result.quantized_4},
    };
}

} // namespace pdi::labs

/**
 * @file main.cpp
 * @brief Integrated executable for Laboratory M2.3.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/io/processing_data_storage.hpp"
#include "pdi/morphology/morphological_pipeline.hpp"
#include "pdi/version.hpp"

#ifdef PDI_HAS_INTERACTIVE_UI
#include "pdi/ui/interactive_window.hpp"
#endif

#include <opencv2/core.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    std::string sequence = "noise-removal";
    std::string element = "square";
    pdi::segmentation::Connectivity connectivity =
        pdi::segmentation::Connectivity::Eight;
    bool compare_with_opencv = false;
    bool show = false;
    bool interactive = false;
    bool save_data = false;
};

struct Execution {
    pdi::morphology::MorphologicalPipelineConfig config;
    pdi::morphology::MorphologicalPipelineResult result;
};

[[nodiscard]] std::vector<pdi::morphology::MorphologicalOperation>
sequence_from_name(const std::string& name) {
    using Operation = pdi::morphology::MorphologicalOperation;

    if (name == "noise-removal" || name == "opening") {
        return {Operation::Erode, Operation::Dilate};
    }

    if (name == "hole-filling" || name == "closing") {
        return {Operation::Dilate, Operation::Erode};
    }

    if (name == "open-close") {
        return {Operation::Open, Operation::Close};
    }

    if (name == "close-open") {
        return {Operation::Close, Operation::Open};
    }

    throw std::invalid_argument(
        "Unsupported sequence. Use noise-removal, hole-filling, "
        "open-close, or close-open."
    );
}

[[nodiscard]] pdi::morphology::BinaryStructuringElement
element_from_name(const std::string& name) {
    if (name == "square") {
        return pdi::morphology::BinaryStructuringElement::square_3x3();
    }

    if (name == "cross") {
        return pdi::morphology::BinaryStructuringElement::cross_3x3();
    }

    throw std::invalid_argument(
        "Unsupported element. Use square or cross."
    );
}

[[nodiscard]] Options parse_options(int argc, char** argv) {
    if (argc < 3) {
        throw std::invalid_argument(
            "Usage: lab_m2_3 <input> <output-directory> "
            "[--sequence name] [--element square|cross] "
            "[--connectivity 4|8] [--compare-opencv] [--show] "
            "[--interactive] [--save-data]"
        );
    }

    Options options{
        .input_path = argv[1],
        .output_directory = argv[2],
    };

    for (int index = 3; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--sequence" && index + 1 < argc) {
            options.sequence = argv[++index];
        } else if (argument == "--element" && index + 1 < argc) {
            options.element = argv[++index];
        } else if (argument == "--connectivity" && index + 1 < argc) {
            const std::string value = argv[++index];

            if (value == "4") {
                options.connectivity =
                    pdi::segmentation::Connectivity::Four;
            } else if (value == "8") {
                options.connectivity =
                    pdi::segmentation::Connectivity::Eight;
            } else {
                throw std::invalid_argument(
                    "Connectivity must be 4 or 8."
                );
            }
        } else if (argument == "--compare-opencv") {
            options.compare_with_opencv = true;
        } else if (argument == "--show") {
            options.show = true;
        } else if (argument == "--interactive") {
            options.interactive = true;
        } else if (argument == "--save-data") {
            options.save_data = true;
        } else {
            throw std::invalid_argument(
                "Unknown or incomplete argument: " + argument
            );
        }
    }

    return options;
}

[[nodiscard]] pdi::morphology::MorphologicalPipelineConfig make_config(
    const std::string& sequence_name,
    const std::string& element_name,
    const pdi::segmentation::Connectivity connectivity,
    const bool compare_with_opencv
) {
    return {
        .element = element_from_name(element_name),
        .sequence = sequence_from_name(sequence_name),
        .border_strategy =
            pdi::morphology::MorphologyBorderStrategy::OutsideBackground,
        .connectivity = connectivity,
        .compare_with_opencv = compare_with_opencv,
    };
}

void print_report(
    const pdi::morphology::MorphologicalPipelineResult& result,
    const pdi::morphology::MorphologicalPipelineConfig& config
) {
    std::cout
        << "Sequence: "
        << pdi::morphology::MorphologicalPipeline::sequence_name(
               config.sequence
           )
        << '\n'
        << "Foreground area: "
        << result.foreground_area_before
        << " -> "
        << result.foreground_area_after
        << '\n'
        << "Components: "
        << result.component_count_before
        << " -> "
        << result.component_count_after
        << '\n'
        << "Removed components: "
        << result.removed_component_count
        << '\n';

    if (result.compared_with_opencv) {
        std::cout
            << "Matches OpenCV: "
            << (result.matches_opencv ? "yes" : "no")
            << '\n';
    }
}

void save_visual_outputs(
    const std::filesystem::path& output_directory,
    const pdi::morphology::MorphologicalPipelineResult& result
) {
    const pdi::io::ImageFileStorage storage;

    storage.save(output_directory / "input.png", result.input);

    for (std::size_t index = 0;
         index < result.intermediate_images.size();
         ++index) {
        storage.save(
            output_directory
                / ("stage_" + std::to_string(index + 1) + ".png"),
            result.intermediate_images[index]
        );
    }

    storage.save(output_directory / "output.png", result.output);
}

void show_outputs(
    const pdi::morphology::MorphologicalPipelineResult& result
) {
    std::vector<pdi::io::WindowImage> images{
        {"M2.3 - input", result.input},
    };

    for (std::size_t index = 0;
         index < result.intermediate_images.size();
         ++index) {
        images.push_back({
            "M2.3 - stage " + std::to_string(index + 1),
            result.intermediate_images[index],
        });
    }

    images.push_back({"M2.3 - output", result.output});
    pdi::io::ImageDisplay{}.show_all(images);
}

#ifdef PDI_HAS_INTERACTIVE_UI
[[nodiscard]] Execution run_interactive(
    const cv::Mat& binary_image,
    pdi::morphology::MorphologicalPipelineConfig config
) {
    pdi::ui::InteractiveWindow window("Laboratory M2.3");
    const pdi::morphology::MorphologicalPipeline pipeline;
    Execution execution{
        .config = config,
        .result = pipeline.run(binary_image, config),
    };

    int sequence_index = 0;
    int element_index =
        cv::countNonZero(config.element.mask) == 5 ? 1 : 0;
    int connectivity_index =
        config.connectivity == pdi::segmentation::Connectivity::Eight
            ? 1
            : 0;

    const auto recompute = [&]() {
        const std::vector<std::string> sequences{
            "noise-removal",
            "hole-filling",
            "open-close",
            "close-open",
        };

        execution.config = make_config(
            sequences[static_cast<std::size_t>(sequence_index)],
            element_index == 0 ? "square" : "cross",
            connectivity_index == 0
                ? pdi::segmentation::Connectivity::Four
                : pdi::segmentation::Connectivity::Eight,
            config.compare_with_opencv
        );
        execution.result = pipeline.run(
            binary_image,
            execution.config
        );
        window.set_image(execution.result.output);
        print_report(execution.result, execution.config);
    };

    window.add_trackbar(
        "Sequence: 0 noise, 1 holes, 2 open-close, 3 close-open",
        sequence_index,
        3,
        [&](const int value) {
            sequence_index = value;
            recompute();
        }
    );
    window.add_trackbar(
        "Element: 0 square, 1 cross",
        element_index,
        1,
        [&](const int value) {
            element_index = value;
            recompute();
        }
    );
    window.add_trackbar(
        "Connectivity: 0 four, 1 eight",
        connectivity_index,
        1,
        [&](const int value) {
            connectivity_index = value;
            recompute();
        }
    );

    recompute();
    window.run();
    return execution;
}
#endif

} // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parse_options(argc, argv);
        const cv::Mat binary_image =
            pdi::io::ImageFileStorage{}.load_grayscale(
                options.input_path
            );

        Execution execution{
            .config = make_config(
                options.sequence,
                options.element,
                options.connectivity,
                options.compare_with_opencv
            ),
            .result = {},
        };

        execution.result =
            pdi::morphology::MorphologicalPipeline{}.run(
                binary_image,
                execution.config
            );

        if (options.interactive) {
#ifdef PDI_HAS_INTERACTIVE_UI
            execution = run_interactive(
                binary_image,
                execution.config
            );
#else
            std::cerr
                << "Interactive mode was requested, but optional UI "
                << "infrastructure is not enabled. Continuing headless.\n";
#endif
        }

        print_report(execution.result, execution.config);
        save_visual_outputs(
            options.output_directory,
            execution.result
        );

        if (options.save_data) {
            auto record =
                pdi::morphology::MorphologicalPipeline{}
                    .make_processing_record(
                        execution.result,
                        execution.config,
                        PDI_PROJECT_VERSION,
                        options.input_path.string()
                    );

            record.parameters.push_back({
                "show",
                options.show ? "true" : "false",
            });
            record.parameters.push_back({
                "interactive",
                options.interactive ? "true" : "false",
            });

            pdi::io::ProcessingDataStorage{}.save_yaml(
                options.output_directory / "m2_3_result.yml",
                record
            );
        }

        if (options.show) {
            show_outputs(execution.result);
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "lab_m2_3: " << exception.what() << '\n';
        return 1;
    }
}

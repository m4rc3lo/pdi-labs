/**
 * @file main.cpp
 * @brief Runs the integrated Laboratory M2.1 application.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/io/processing_data_storage.hpp"
#include "pdi/labs/m2_1_pipeline.hpp"
#include "pdi/version.hpp"

#ifdef PDI_HAS_INTERACTIVE_UI
#include "pdi/ui/interactive_state.hpp"
#include "pdi/ui/interactive_window.hpp"
#include "pdi/ui/qt_controls.hpp"
#include "pdi/ui/ui_capabilities.hpp"
#endif

#include <opencv2/core.hpp>

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using Operation = pdi::labs::M21Operation;

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    Operation operation;
    std::filesystem::path secondary_path;
    int threshold = 128;
    int minimum_value = 80;
    int maximum_value = 160;
    int adaptive_block_size = 11;
    double adaptive_constant = 2.0;
    int distance_mask_size = 3;
    double foreground_ratio = 0.4;
    bool show = false;
    bool interactive = false;
    bool save_data = false;
};

[[nodiscard]] Operation parse_operation(const std::string& value) {
    if (value == "manual-global") return Operation::ManualGlobal;
    if (value == "manual-interval") return Operation::ManualInterval;
    if (value == "mask-invert") return Operation::MaskInvert;
    if (value == "mask-intersection") return Operation::MaskIntersection;
    if (value == "mask-union") return Operation::MaskUnion;
    if (value == "mask-difference") return Operation::MaskDifference;
    if (value == "mask-apply-gray") return Operation::MaskApplyGrayscale;
    if (value == "mask-apply-color") return Operation::MaskApplyColor;
    if (value == "hsv") return Operation::BgrToHsv;
    if (value == "otsu") return Operation::Otsu;
    if (value == "adaptive") return Operation::AdaptiveMean;
    if (value == "distance") return Operation::DistanceTransform;
    if (value == "watershed") return Operation::Watershed;

    throw std::invalid_argument("Unsupported operation: " + value);
}

[[nodiscard]] int parse_byte(
    const std::string& value,
    const std::string& option_name
) {
    const int parsed = std::stoi(value);
    if (parsed < 0 || parsed > 255) {
        throw std::invalid_argument(option_name + " must be in [0, 255].");
    }
    return parsed;
}

[[nodiscard]] Options parse_options(int argc, char* argv[]) {
    if (argc < 4) {
        throw std::invalid_argument(
            "Usage: lab_m2_1 <input> <output-directory> <operation> "
            "[--second <mask>] [--threshold <0..255>] "
            "[--min <0..255>] [--max <0..255>] "
            "[--block-size N] [--constant C] [--distance-mask <3|5>] "
            "[--foreground-ratio R] [--show] [--interactive] "
            "[--save-data]"
        );
    }

    Options options{
        .input_path = argv[1],
        .output_directory = argv[2],
        .operation = parse_operation(argv[3]),
        .secondary_path = {},
    };

    for (int index = 4; index < argc; ++index) {
        const std::string argument = argv[index];

        const auto require_value = [&]() -> std::string {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after " + argument + "."
                );
            }
            return argv[++index];
        };

        if (argument == "--second") {
            options.secondary_path = require_value();
        } else if (argument == "--threshold") {
            options.threshold = parse_byte(require_value(), argument);
        } else if (argument == "--min") {
            options.minimum_value = parse_byte(require_value(), argument);
        } else if (argument == "--max") {
            options.maximum_value = parse_byte(require_value(), argument);
        } else if (argument == "--block-size") {
            options.adaptive_block_size = std::stoi(require_value());
        } else if (argument == "--constant") {
            options.adaptive_constant = std::stod(require_value());
        } else if (argument == "--distance-mask") {
            options.distance_mask_size = std::stoi(require_value());
        } else if (argument == "--foreground-ratio") {
            options.foreground_ratio = std::stod(require_value());
        } else if (argument == "--show") {
            options.show = true;
        } else if (argument == "--interactive") {
            options.interactive = true;
        } else if (argument == "--save-data") {
            options.save_data = true;
        } else {
            throw std::invalid_argument("Unsupported argument: " + argument);
        }
    }

    if (options.minimum_value > options.maximum_value) {
        throw std::invalid_argument("--min must not exceed --max.");
    }
    if (pdi::labs::M21Pipeline::requires_secondary_input(options.operation)
        && options.secondary_path.empty()) {
        throw std::invalid_argument(
            "The selected operation requires --second <mask>."
        );
    }
    if (options.interactive
        && options.operation != Operation::ManualGlobal
        && options.operation != Operation::ManualInterval) {
        throw std::invalid_argument(
            "--interactive is available for manual-global and "
            "manual-interval."
        );
    }

    return options;
}

[[nodiscard]] pdi::labs::M21Parameters parameters_from(
    const Options& options
) {
    return {
        .operation = options.operation,
        .threshold = options.threshold,
        .minimum_value = options.minimum_value,
        .maximum_value = options.maximum_value,
        .adaptive_block_size = options.adaptive_block_size,
        .adaptive_constant = options.adaptive_constant,
        .distance_mask_size = options.distance_mask_size,
        .foreground_ratio = options.foreground_ratio,
    };
}

void save_result(
    const Options& options,
    const pdi::labs::M21PipelineResult& result,
    const pdi::labs::M21Parameters& parameters
) {
    const pdi::io::ImageFileStorage image_storage;

    for (const auto& output : result.visual_outputs) {
        image_storage.save(
            options.output_directory / (output.name + ".png"),
            output.image
        );
    }

    if (!options.save_data) {
        return;
    }

    auto recorded_parameters = result.parameters;
    recorded_parameters.push_back({
        "interaction_mode",
        options.interactive ? "interactive" : "headless",
    });
    recorded_parameters.push_back({
        "configured_threshold",
        std::to_string(parameters.threshold),
    });
    recorded_parameters.push_back({
        "configured_minimum",
        std::to_string(parameters.minimum_value),
    });
    recorded_parameters.push_back({
        "configured_maximum",
        std::to_string(parameters.maximum_value),
    });
    if (!options.secondary_path.empty()) {
        recorded_parameters.push_back({
            "secondary_input_path",
            options.secondary_path.string(),
        });
    }

    pdi::io::ProcessingDataStorage{}.save_yaml(
        options.output_directory
            / ("m2_1_" + result.operation_name + ".yml"),
        {
            .format_version = "1",
            .project_version = PDI_PROJECT_VERSION,
            .laboratory = "M2.1",
            .operation = result.operation_name,
            .input_path = options.input_path.string(),
            .parameters = std::move(recorded_parameters),
            .numeric_artifacts = result.numeric_artifacts,
        }
    );
}

void show_result(
    const cv::Mat& input,
    const cv::Mat& secondary,
    const pdi::labs::M21PipelineResult& result
) {
    std::vector<pdi::io::WindowImage> windows{
        {"M2.1 input", input},
    };

    if (!secondary.empty()) {
        windows.push_back({"M2.1 secondary mask", secondary});
    }
    for (const auto& output : result.visual_outputs) {
        windows.push_back({"M2.1 " + output.name, output.image});
    }

    pdi::io::ImageDisplay{}.show_all(windows);
}

#ifdef PDI_HAS_INTERACTIVE_UI
void run_interactive(
    const Options& options,
    const cv::Mat& input,
    pdi::labs::M21Parameters& parameters
) {
    pdi::ui::InteractiveState state;
    state.define_parameter("threshold", parameters.threshold, 0, 255);
    state.define_parameter("minimum", parameters.minimum_value, 0, 255);
    state.define_parameter("maximum", parameters.maximum_value, 0, 255);

    Operation operation = parameters.operation;
    pdi::labs::M21PipelineResult result;
    pdi::ui::InteractiveWindow window{"M2.1 manual threshold"};

    const auto refresh = [&]() {
        parameters.operation = operation;
        parameters.threshold = state.parameter("threshold");
        parameters.minimum_value = state.parameter("minimum");
        parameters.maximum_value = state.parameter("maximum");
        result = pdi::labs::M21Pipeline{}.run(
            input,
            cv::Mat{},
            parameters
        );
        window.set_image(result.visual_outputs.front().image);
    };

    window.add_trackbar(
        "Threshold",
        parameters.threshold,
        255,
        [&](int value) {
            state.set_parameter("threshold", value);
            refresh();
        }
    );
    window.add_trackbar(
        "Minimum",
        parameters.minimum_value,
        255,
        [&](int value) {
            int maximum = state.parameter("maximum");
            if (value > maximum) {
                maximum = value;
                window.set_trackbar_position("Maximum", maximum);
            }
            state.set_ordered_pair("minimum", "maximum", value, maximum);
            refresh();
        }
    );
    window.add_trackbar(
        "Maximum",
        parameters.maximum_value,
        255,
        [&](int value) {
            int minimum = state.parameter("minimum");
            if (value < minimum) {
                minimum = value;
                window.set_trackbar_position("Minimum", minimum);
            }
            state.set_ordered_pair("minimum", "maximum", minimum, value);
            refresh();
        }
    );

    window.set_mouse_callback(
        [&](const pdi::ui::MouseEvent& event) {
            if (event.action != pdi::ui::MouseAction::LeftButtonDown
                || event.x < 0
                || event.y < 0
                || event.x >= input.cols
                || event.y >= input.rows) {
                return;
            }
            const auto* row = input.ptr<std::uint8_t>(event.y);
            std::cout
                << "Pixel (" << event.x << ", " << event.y << ") = "
                << static_cast<int>(row[event.x]) << '\n';
        }
    );

    window.set_keyboard_callback(
        [&](const pdi::ui::KeyboardEvent& event) {
            if (event.command == pdi::ui::KeyboardCommand::Save) {
                save_result(options, result, parameters);
            } else if (event.command
                       == pdi::ui::KeyboardCommand::Reset) {
                state.set_parameter("threshold", 128);
                state.set_ordered_pair("minimum", "maximum", 80, 160);
                window.set_trackbar_position("Threshold", 128);
                window.set_trackbar_position("Minimum", 80);
                window.set_trackbar_position("Maximum", 160);
                refresh();
            } else if (event.command
                       == pdi::ui::KeyboardCommand::ToggleMode) {
                operation = operation == Operation::ManualGlobal
                    ? Operation::ManualInterval
                    : Operation::ManualGlobal;
                std::cout
                    << "Mode: "
                    << (operation == Operation::ManualGlobal
                            ? "manual-global"
                            : "manual-interval")
                    << '\n';
                refresh();
            }
        }
    );

    pdi::ui::QtControls controls;
    if (pdi::ui::UiCapabilities::detect().qt_controls) {
        static_cast<void>(controls.add_checkbox(
            "Interval mode",
            operation == Operation::ManualInterval,
            [&](bool enabled) {
                operation = enabled
                    ? Operation::ManualInterval
                    : Operation::ManualGlobal;
                refresh();
            }
        ));
        static_cast<void>(controls.add_push_button(
            "Save",
            [&]() {
                save_result(options, result, parameters);
            }
        ));
    }

    std::cout
        << "Interactive keys: M mode, R reset, S save, "
        << "Q or Esc exit. Click to inspect a pixel.\n";
    refresh();
    window.run();
    pdi::ui::InteractiveWindow::destroy_all_windows();
    save_result(options, result, parameters);
}
#endif

} // namespace

int main(int argc, char* argv[]) {
    try {
        const Options options = parse_options(argc, argv);
        const pdi::io::ImageFileStorage storage;

        const cv::Mat input =
            pdi::labs::M21Pipeline::requires_color_input(options.operation)
                ? storage.load_color(options.input_path)
                : storage.load_grayscale(options.input_path);
        const cv::Mat secondary =
            options.secondary_path.empty()
                ? cv::Mat{}
                : storage.load_grayscale(options.secondary_path);

        auto parameters = parameters_from(options);

        if (options.interactive) {
#ifdef PDI_HAS_INTERACTIVE_UI
            run_interactive(options, input, parameters);
            return 0;
#else
            throw std::runtime_error(
                "Interactive UI support is not compiled. Reconfigure with "
                "-DPDI_BUILD_INTERACTIVE_UI=ON."
            );
#endif
        }

        const auto result = pdi::labs::M21Pipeline{}.run(
            input,
            secondary,
            parameters
        );
        save_result(options, result, parameters);

        if (options.show) {
            show_result(input, secondary, result);
        }

        std::cout
            << "Operation: " << result.operation_name << '\n'
            << "Visual outputs: " << result.visual_outputs.size() << '\n'
            << "Numeric artifacts: "
            << result.numeric_artifacts.size() << '\n';
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

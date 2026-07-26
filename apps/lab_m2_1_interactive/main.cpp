/**
 * @file main.cpp
 * @brief Demonstrates optional interactive controls for manual thresholding.
 */

#include "pdi/io/image_display.hpp"
#include "pdi/io/image_file_storage.hpp"
#include "pdi/io/processing_data_storage.hpp"
#include "pdi/segmentation/manual_threshold.hpp"
#include "pdi/ui/interactive_state.hpp"
#include "pdi/ui/interactive_window.hpp"
#include "pdi/ui/qt_controls.hpp"
#include "pdi/ui/ui_capabilities.hpp"
#include "pdi/version.hpp"

#include <opencv2/core.hpp>

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

enum class ThresholdMode {
    Global,
    Interval,
};

struct Options {
    std::filesystem::path input_path;
    std::filesystem::path output_directory;
    ThresholdMode mode;
    int threshold;
    int minimum_value;
    int maximum_value;
    bool show;
    bool interactive;
    bool save_data;
};

[[nodiscard]] int parse_byte(
    const std::string& text,
    const std::string& option_name
) {
    const int value = std::stoi(text);

    if (value < 0 || value > 255) {
        throw std::invalid_argument(
            option_name + " must be in [0, 255]."
        );
    }

    return value;
}

[[nodiscard]] Options parse_options(int argc, char* argv[]) {
    if (argc < 3) {
        throw std::invalid_argument(
            "Usage: lab_m2_1_interactive <input> <output-directory> "
            "[--mode <global|interval>] [--threshold <0..255>] "
            "[--min <0..255>] [--max <0..255>] "
            "[--show] [--interactive] [--save-data]"
        );
    }

    Options options{
        .input_path = argv[1],
        .output_directory = argv[2],
        .mode = ThresholdMode::Global,
        .threshold = 128,
        .minimum_value = 80,
        .maximum_value = 160,
        .show = false,
        .interactive = false,
        .save_data = false,
    };

    for (int index = 3; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--mode") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --mode."
                );
            }

            const std::string value = argv[++index];

            if (value == "global") {
                options.mode = ThresholdMode::Global;
            } else if (value == "interval") {
                options.mode = ThresholdMode::Interval;
            } else {
                throw std::invalid_argument(
                    "Unsupported threshold mode: " + value
                );
            }
        } else if (argument == "--threshold") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --threshold."
                );
            }
            options.threshold = parse_byte(
                argv[++index],
                "--threshold"
            );
        } else if (argument == "--min") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --min."
                );
            }
            options.minimum_value = parse_byte(argv[++index], "--min");
        } else if (argument == "--max") {
            if (index + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value after --max."
                );
            }
            options.maximum_value = parse_byte(argv[++index], "--max");
        } else if (argument == "--show") {
            options.show = true;
        } else if (argument == "--interactive") {
            options.interactive = true;
        } else if (argument == "--save-data") {
            options.save_data = true;
        } else {
            throw std::invalid_argument(
                "Unsupported argument: " + argument
            );
        }
    }

    if (options.minimum_value > options.maximum_value) {
        throw std::invalid_argument(
            "--min must not exceed --max."
        );
    }

    return options;
}

[[nodiscard]] std::string mode_name(ThresholdMode mode) {
    return mode == ThresholdMode::Global ? "global" : "interval";
}

[[nodiscard]] cv::Mat process(
    const cv::Mat& input,
    ThresholdMode mode,
    const pdi::ui::InteractiveState& state
) {
    const pdi::segmentation::ManualThreshold thresholding;

    if (mode == ThresholdMode::Global) {
        return thresholding.binary_global(
            input,
            static_cast<std::uint8_t>(
                state.parameter("threshold")
            )
        );
    }

    return thresholding.select_interval(
        input,
        static_cast<std::uint8_t>(state.parameter("minimum")),
        static_cast<std::uint8_t>(state.parameter("maximum"))
    );
}

void save_current(
    const Options& options,
    ThresholdMode mode,
    const pdi::ui::InteractiveState& state,
    const cv::Mat& output
) {
    const pdi::io::ImageFileStorage image_storage;
    const auto image_path =
        options.output_directory
        / ("threshold_" + mode_name(mode) + ".png");

    image_storage.save(image_path, output);
    std::cout << "Saved: " << image_path.string() << '\n';

    if (!options.save_data) {
        return;
    }

    pdi::io::ProcessingDataStorage{}.save_yaml(
        options.output_directory / "threshold_result.yml",
        {
            .format_version = "1",
            .project_version = PDI_PROJECT_VERSION,
            .laboratory = "M2.1",
            .operation = "manual_threshold_" + mode_name(mode),
            .input_path = options.input_path.string(),
            .parameters = {
                {"interaction_mode",
                 options.interactive ? "interactive" : "static"},
                {"threshold",
                 std::to_string(state.parameter("threshold"))},
                {"minimum",
                 std::to_string(state.parameter("minimum"))},
                {"maximum",
                 std::to_string(state.parameter("maximum"))},
            },
            .numeric_artifacts = {},
        }
    );
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const Options options = parse_options(argc, argv);
        const pdi::io::ImageFileStorage storage;
        const cv::Mat input = storage.load_grayscale(options.input_path);

        pdi::ui::InteractiveState state;
        state.define_parameter("threshold", options.threshold, 0, 255);
        state.define_parameter("minimum", options.minimum_value, 0, 255);
        state.define_parameter("maximum", options.maximum_value, 0, 255);

        ThresholdMode mode = options.mode;
        cv::Mat output = process(input, mode, state);
        save_current(options, mode, state, output);

        if (options.show && !options.interactive) {
            pdi::io::ImageDisplay{}.show_all({
                {"Input grayscale", input},
                {"Manual threshold", output},
            });
        }

        if (!options.interactive) {
            return 0;
        }

        const auto capabilities =
            pdi::ui::UiCapabilities::detect();

        std::cout
            << "HighGUI backend: " << capabilities.backend_name << '\n'
            << "Keys: M mode, R reset, S save, Q or Esc exit.\n"
            << "Click the image to inspect the input intensity.\n";

        pdi::ui::InteractiveWindow window{
            "M2.1 manual threshold"
        };

        auto refresh = [&]() {
            output = process(input, mode, state);
            window.set_image(output);
        };

        window.add_trackbar(
            "Threshold",
            state.parameter("threshold"),
            255,
            [&](int value) {
                state.set_parameter("threshold", value);
                refresh();
            }
        );

        window.add_trackbar(
            "Minimum",
            state.parameter("minimum"),
            255,
            [&](int value) {
                int maximum = state.parameter("maximum");

                if (value > maximum) {
                    maximum = value;
                    window.set_trackbar_position(
                        "Maximum",
                        maximum
                    );
                }

                state.set_ordered_pair(
                    "minimum",
                    "maximum",
                    value,
                    maximum
                );
                refresh();
            }
        );

        window.add_trackbar(
            "Maximum",
            state.parameter("maximum"),
            255,
            [&](int value) {
                int minimum = state.parameter("minimum");

                if (value < minimum) {
                    minimum = value;
                    window.set_trackbar_position(
                        "Minimum",
                        minimum
                    );
                }

                state.set_ordered_pair(
                    "minimum",
                    "maximum",
                    minimum,
                    value
                );
                refresh();
            }
        );

        window.set_mouse_callback(
            [&](const pdi::ui::MouseEvent& event) {
                if (event.action
                        != pdi::ui::MouseAction::LeftButtonDown
                    || event.x < 0
                    || event.y < 0
                    || event.x >= input.cols
                    || event.y >= input.rows) {
                    return;
                }

                const auto* row =
                    input.ptr<std::uint8_t>(event.y);
                std::cout
                    << "Pixel (" << event.x << ", " << event.y
                    << ") = " << static_cast<int>(row[event.x])
                    << '\n';
            }
        );

        window.set_keyboard_callback(
            [&](const pdi::ui::KeyboardEvent& event) {
                switch (event.command) {
                case pdi::ui::KeyboardCommand::Exit:
                    state.request_close();
                    window.request_close();
                    break;

                case pdi::ui::KeyboardCommand::Save:
                    state.request_save();
                    save_current(options, mode, state, output);
                    break;

                case pdi::ui::KeyboardCommand::Reset:
                    state.set_parameter("threshold", 128);
                    state.set_ordered_pair(
                        "minimum",
                        "maximum",
                        80,
                        160
                    );
                    window.set_trackbar_position("Threshold", 128);
                    window.set_trackbar_position("Minimum", 80);
                    window.set_trackbar_position("Maximum", 160);
                    refresh();
                    break;

                case pdi::ui::KeyboardCommand::ToggleMode:
                    mode = mode == ThresholdMode::Global
                        ? ThresholdMode::Interval
                        : ThresholdMode::Global;
                    std::cout
                        << "Mode: " << mode_name(mode) << '\n';
                    refresh();
                    break;

                case pdi::ui::KeyboardCommand::None:
                    break;
                }
            }
        );

        pdi::ui::QtControls qt_controls;

        if (capabilities.qt_controls) {
            static_cast<void>(
                qt_controls.add_checkbox(
                    "Interval mode",
                    mode == ThresholdMode::Interval,
                    [&](bool enabled) {
                        mode = enabled
                            ? ThresholdMode::Interval
                            : ThresholdMode::Global;
                        refresh();
                    }
                )
            );

            static_cast<void>(
                qt_controls.add_push_button(
                    "Save",
                    [&]() {
                        save_current(
                            options,
                            mode,
                            state,
                            output
                        );
                    }
                )
            );
        } else {
            std::cout
                << "Qt controls unavailable; using trackbars, "
                << "mouse and keyboard only.\n";
        }

        refresh();
        window.run();
        pdi::ui::InteractiveWindow::destroy_all_windows();
        save_current(options, mode, state, output);
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}

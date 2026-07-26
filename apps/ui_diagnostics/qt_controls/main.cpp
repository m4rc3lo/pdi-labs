/**
 * @file main.cpp
 * @brief Manual diagnostic for Qt-only HighGUI controls.
 */

#include "pdi/ui/interactive_window.hpp"
#include "pdi/ui/qt_controls.hpp"
#include "pdi/ui/ui_capabilities.hpp"

#include <opencv2/core.hpp>

#include <iostream>

int main() {
    const auto capabilities = pdi::ui::UiCapabilities::detect();

    std::cout
        << "Backend: " << capabilities.backend_name << '\n'
        << "Qt controls: "
        << (capabilities.qt_controls ? "available" : "unavailable")
        << '\n';

    if (!capabilities.qt_controls) {
        return 0;
    }

    pdi::ui::InteractiveWindow window{"Qt controls diagnostic"};
    pdi::ui::QtControls controls;
    const cv::Mat image(240, 320, CV_8UC1, cv::Scalar{128});

    static_cast<void>(
        controls.add_push_button(
            "Print",
            []() {
                std::cout << "Push button activated.\n";
            }
        )
    );
    static_cast<void>(
        controls.add_checkbox(
            "Checkbox",
            false,
            [](bool checked) {
                std::cout
                    << "Checkbox: "
                    << (checked ? "on" : "off")
                    << '\n';
            }
        )
    );
    static_cast<void>(
        controls.add_radio_button(
            "Radio",
            false,
            [](bool checked) {
                std::cout
                    << "Radio: "
                    << (checked ? "on" : "off")
                    << '\n';
            }
        )
    );

    window.set_keyboard_callback(
        [&](const pdi::ui::KeyboardEvent& event) {
            if (event.command == pdi::ui::KeyboardCommand::Exit) {
                window.request_close();
            }
        }
    );

    window.set_image(image);
    window.run();
    pdi::ui::InteractiveWindow::destroy_all_windows();
    return 0;
}

/**
 * @file main.cpp
 * @brief Manual diagnostic for HighGUI trackbars.
 */

#include "pdi/ui/interactive_window.hpp"
#include "pdi/ui/ui_capabilities.hpp"

#include <opencv2/core.hpp>

#include <iostream>

int main() {
    const auto capabilities = pdi::ui::UiCapabilities::detect();
    std::cout << "Backend: " << capabilities.backend_name << '\n';

    pdi::ui::InteractiveWindow window{"Trackbar diagnostic"};
    cv::Mat image(240, 320, CV_8UC1, cv::Scalar{128});

    window.add_trackbar(
        "Intensity",
        128,
        255,
        [&](int value) {
            image.setTo(
                cv::Scalar{static_cast<double>(value)}
            );
            window.set_image(image);
        }
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
    return 0;
}

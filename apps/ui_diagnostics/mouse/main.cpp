/**
 * @file main.cpp
 * @brief Manual diagnostic for HighGUI mouse callbacks.
 */

#include "pdi/ui/interactive_window.hpp"

#include <opencv2/core.hpp>

#include <iostream>

int main() {
    pdi::ui::InteractiveWindow window{"Mouse diagnostic"};
    const cv::Mat image(240, 320, CV_8UC1, cv::Scalar{128});

    window.set_mouse_callback(
        [](const pdi::ui::MouseEvent& event) {
            std::cout
                << "Mouse: action="
                << static_cast<int>(event.action)
                << " x=" << event.x
                << " y=" << event.y
                << " flags=" << event.flags
                << '\n';
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

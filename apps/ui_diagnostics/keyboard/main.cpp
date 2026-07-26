/**
 * @file main.cpp
 * @brief Manual diagnostic for HighGUI keyboard events.
 */

#include "pdi/ui/interactive_window.hpp"

#include <opencv2/core.hpp>

#include <iostream>

int main() {
    pdi::ui::InteractiveWindow window{"Keyboard diagnostic"};
    const cv::Mat image(240, 320, CV_8UC1, cv::Scalar{128});

    window.set_keyboard_callback(
        [&](const pdi::ui::KeyboardEvent& event) {
            std::cout
                << "Key: raw=" << event.raw_code
                << " normalized=" << event.normalized_code
                << " command=" << static_cast<int>(event.command)
                << '\n';

            if (event.command == pdi::ui::KeyboardCommand::Exit) {
                window.request_close();
            }
        }
    );

    window.set_image(image);
    window.run();
    return 0;
}

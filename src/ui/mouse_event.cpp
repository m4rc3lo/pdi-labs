/**
 * @file mouse_event.cpp
 * @brief Implements typed mouse event translation.
 */

#include "pdi/ui/mouse_event.hpp"

#include <opencv2/highgui.hpp>

namespace pdi::ui {

MouseEvent MouseEvent::from_highgui(
    int raw_event,
    int x,
    int y,
    int flags
) {
    MouseAction action = MouseAction::Unknown;

    switch (raw_event) {
    case cv::EVENT_MOUSEMOVE:
        action = MouseAction::Move;
        break;
    case cv::EVENT_LBUTTONDOWN:
        action = MouseAction::LeftButtonDown;
        break;
    case cv::EVENT_LBUTTONUP:
        action = MouseAction::LeftButtonUp;
        break;
    case cv::EVENT_RBUTTONDOWN:
        action = MouseAction::RightButtonDown;
        break;
    case cv::EVENT_RBUTTONUP:
        action = MouseAction::RightButtonUp;
        break;
    case cv::EVENT_MBUTTONDOWN:
        action = MouseAction::MiddleButtonDown;
        break;
    case cv::EVENT_MBUTTONUP:
        action = MouseAction::MiddleButtonUp;
        break;
    case cv::EVENT_MOUSEWHEEL:
        action = MouseAction::MouseWheel;
        break;
    case cv::EVENT_MOUSEHWHEEL:
        action = MouseAction::HorizontalWheel;
        break;
    default:
        break;
    }

    return {
        .action = action,
        .x = x,
        .y = y,
        .flags = flags,
        .raw_event = raw_event,
    };
}

} // namespace pdi::ui

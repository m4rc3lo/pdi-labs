/**
 * @file interactive_ui_test.cpp
 * @brief Tests GUI-independent interactive state and event translation.
 */

#include "pdi/ui/interactive_state.hpp"
#include "pdi/ui/keyboard_event_dispatcher.hpp"
#include "pdi/ui/keyboard_event.hpp"
#include "pdi/ui/mouse_event.hpp"
#include "pdi/ui/ui_capabilities.hpp"

#include <catch2/catch_test_macros.hpp>

#include <opencv2/highgui.hpp>

TEST_CASE(
    "InteractiveState validates parameters and requests",
    "[unit][ui][state]"
) {
    pdi::ui::InteractiveState state;
    state.define_parameter("threshold", 128, 0, 255);
    state.define_parameter("minimum", 80, 0, 255);
    state.define_parameter("maximum", 160, 0, 255);

    REQUIRE(state.parameter("threshold") == 128);
    REQUIRE_FALSE(state.consume_reprocess_request());

    state.set_parameter("threshold", 200);
    REQUIRE(state.parameter("threshold") == 200);
    REQUIRE(state.consume_reprocess_request());
    REQUIRE_FALSE(state.consume_reprocess_request());

    state.request_save();
    REQUIRE(state.consume_save_request());
    REQUIRE_FALSE(state.consume_save_request());

    state.request_close();
    REQUIRE(state.close_requested());
}

TEST_CASE(
    "InteractiveState enforces ordered interval",
    "[unit][ui][state][interval]"
) {
    pdi::ui::InteractiveState state;
    state.define_parameter("minimum", 50, 0, 255);
    state.define_parameter("maximum", 150, 0, 255);

    state.set_ordered_pair("minimum", "maximum", 100, 100);

    REQUIRE(state.parameter("minimum") == 100);
    REQUIRE(state.parameter("maximum") == 100);

    REQUIRE_THROWS(
        state.set_ordered_pair(
            "minimum",
            "maximum",
            151,
            150
        )
    );
}

TEST_CASE(
    "KeyboardEvent maps generic commands",
    "[unit][ui][keyboard]"
) {
    REQUIRE(
        pdi::ui::KeyboardEvent::from_raw_code('s').command
        == pdi::ui::KeyboardCommand::Save
    );
    REQUIRE(
        pdi::ui::KeyboardEvent::from_raw_code('M').command
        == pdi::ui::KeyboardCommand::ToggleMode
    );
    REQUIRE(
        pdi::ui::KeyboardEvent::from_raw_code(27).command
        == pdi::ui::KeyboardCommand::Exit
    );
    REQUIRE(
        pdi::ui::KeyboardEvent::from_raw_code(-1).command
        == pdi::ui::KeyboardCommand::None
    );
}


TEST_CASE(
    "KeyboardEvent maps every universal exit key",
    "[unit][ui][keyboard][exit]"
) {
    REQUIRE(
        pdi::ui::KeyboardEvent::from_raw_code(27).command
        == pdi::ui::KeyboardCommand::Exit
    );
    REQUIRE(
        pdi::ui::KeyboardEvent::from_raw_code('q').command
        == pdi::ui::KeyboardCommand::Exit
    );
    REQUIRE(
        pdi::ui::KeyboardEvent::from_raw_code('Q').command
        == pdi::ui::KeyboardCommand::Exit
    );
    REQUIRE(
        pdi::ui::KeyboardEvent::from_raw_code('x').command
        == pdi::ui::KeyboardCommand::None
    );
}

TEST_CASE(
    "Keyboard dispatcher closes without an application callback",
    "[unit][ui][keyboard][dispatcher]"
) {
    pdi::windowing::WindowLifecycleState lifecycle;

    pdi::ui::KeyboardEventDispatcher::dispatch(
        27,
        lifecycle
    );

    REQUIRE_FALSE(lifecycle.should_continue());
    REQUIRE(
        lifecycle.close_reason()
        == pdi::windowing::WindowCloseReason::Keyboard
    );
}

TEST_CASE(
    "Keyboard dispatcher closes before forwarding the exit event",
    "[unit][ui][keyboard][dispatcher][callback]"
) {
    pdi::windowing::WindowLifecycleState lifecycle;
    bool callback_called = false;
    bool already_closed = false;

    pdi::ui::KeyboardEventDispatcher::dispatch(
        'q',
        lifecycle,
        [&](const pdi::ui::KeyboardEvent& event) {
            callback_called = true;
            already_closed = !lifecycle.should_continue();
            REQUIRE(event.command == pdi::ui::KeyboardCommand::Exit);
        }
    );

    REQUIRE(callback_called);
    REQUIRE(already_closed);
}

TEST_CASE(
    "Keyboard dispatcher ignores absence of a key",
    "[unit][ui][keyboard][dispatcher]"
) {
    pdi::windowing::WindowLifecycleState lifecycle;
    bool callback_called = false;

    pdi::ui::KeyboardEventDispatcher::dispatch(
        -1,
        lifecycle,
        [&](const pdi::ui::KeyboardEvent&) {
            callback_called = true;
        }
    );

    REQUIRE(lifecycle.should_continue());
    REQUIRE_FALSE(callback_called);
}

TEST_CASE(
    "MouseEvent preserves coordinates and action",
    "[unit][ui][mouse]"
) {
    const auto event = pdi::ui::MouseEvent::from_highgui(
        cv::EVENT_LBUTTONDOWN,
        12,
        34,
        cv::EVENT_FLAG_CTRLKEY
    );

    REQUIRE(event.action == pdi::ui::MouseAction::LeftButtonDown);
    REQUIRE(event.x == 12);
    REQUIRE(event.y == 34);
    REQUIRE(event.flags == cv::EVENT_FLAG_CTRLKEY);
}

TEST_CASE(
    "UiCapabilities reports a consistent backend state",
    "[unit][ui][capabilities]"
) {
    const auto capabilities = pdi::ui::UiCapabilities::detect();

    REQUIRE_FALSE(capabilities.backend_name.empty());

    if (!capabilities.windows) {
        REQUIRE_FALSE(capabilities.trackbars);
        REQUIRE_FALSE(capabilities.mouse_callbacks);
        REQUIRE_FALSE(capabilities.keyboard_events);
        REQUIRE_FALSE(capabilities.qt_controls);
    }

    if (capabilities.qt_controls) {
        REQUIRE(capabilities.windows);
    }
}

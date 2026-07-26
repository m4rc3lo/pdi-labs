/**
 * @file window_lifecycle_test.cpp
 * @brief Tests GUI-independent window lifecycle decisions.
 */

#include "pdi/windowing/window_lifecycle.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "Window lifecycle recognizes universal exit keys",
    "[unit][windowing][keyboard]"
) {
    using pdi::windowing::WindowLifecycleState;

    REQUIRE(WindowLifecycleState::is_exit_key_code(27));
    REQUIRE(WindowLifecycleState::is_exit_key_code('q'));
    REQUIRE(WindowLifecycleState::is_exit_key_code('Q'));
    REQUIRE_FALSE(WindowLifecycleState::is_exit_key_code('x'));
    REQUIRE_FALSE(WindowLifecycleState::is_exit_key_code(-1));
}

TEST_CASE(
    "Window lifecycle interprets visibility values",
    "[unit][windowing][visibility]"
) {
    using pdi::windowing::WindowLifecycleState;
    using pdi::windowing::WindowVisibility;

    REQUIRE(
        WindowLifecycleState::interpret_visibility(1.0)
        == WindowVisibility::Visible
    );
    REQUIRE(
        WindowLifecycleState::interpret_visibility(0.0)
        == WindowVisibility::Closed
    );
    REQUIRE(
        WindowLifecycleState::interpret_visibility(-1.0)
        == WindowVisibility::Unavailable
    );
}

TEST_CASE(
    "Window lifecycle closes idempotently and preserves first reason",
    "[unit][windowing][state]"
) {
    using pdi::windowing::WindowCloseReason;
    using pdi::windowing::WindowLifecycleState;

    WindowLifecycleState lifecycle;
    REQUIRE(lifecycle.should_continue());

    lifecycle.request_close(WindowCloseReason::Requested);
    lifecycle.request_close(WindowCloseReason::Keyboard);

    REQUIRE_FALSE(lifecycle.should_continue());
    REQUIRE(
        lifecycle.close_reason() == WindowCloseReason::Requested
    );
}

TEST_CASE(
    "Window lifecycle closes without a GUI callback",
    "[unit][windowing][state][keyboard]"
) {
    using pdi::windowing::WindowCloseReason;
    using pdi::windowing::WindowLifecycleState;

    WindowLifecycleState lifecycle;
    lifecycle.observe_key_code('q');

    REQUIRE_FALSE(lifecycle.should_continue());
    REQUIRE(
        lifecycle.close_reason() == WindowCloseReason::Keyboard
    );
}

TEST_CASE(
    "Window lifecycle closes when window becomes unavailable",
    "[unit][windowing][state][visibility]"
) {
    using pdi::windowing::WindowCloseReason;
    using pdi::windowing::WindowLifecycleState;
    using pdi::windowing::WindowVisibility;

    WindowLifecycleState closed;
    closed.observe_visibility(WindowVisibility::Closed);
    REQUIRE_FALSE(closed.should_continue());
    REQUIRE(
        closed.close_reason() == WindowCloseReason::WindowClosed
    );

    WindowLifecycleState unavailable;
    unavailable.observe_visibility(WindowVisibility::Unavailable);
    REQUIRE_FALSE(unavailable.should_continue());
    REQUIRE(
        unavailable.close_reason()
        == WindowCloseReason::BackendUnavailable
    );
}

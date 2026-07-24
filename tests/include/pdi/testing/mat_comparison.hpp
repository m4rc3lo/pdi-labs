/**
 * @file mat_comparison.hpp
 * @brief Provides cv::Mat comparison helpers for automated tests.
 */

#pragma once

#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace pdi::testing {

namespace detail {

template<typename ValueType>
void require_mat_exact_typed(
    const cv::Mat& actual,
    const cv::Mat& expected
) {
    const int scalar_columns = actual.cols * actual.channels();

    for (int row = 0; row < actual.rows; ++row) {
        const auto* actual_row = actual.ptr<ValueType>(row);
        const auto* expected_row = expected.ptr<ValueType>(row);

        for (int scalar_column = 0; scalar_column < scalar_columns; ++scalar_column) {
            const int column = scalar_column / actual.channels();
            const int channel = scalar_column % actual.channels();

            INFO("row=" << row << ", column=" << column << ", channel=" << channel);
            REQUIRE(actual_row[scalar_column] == expected_row[scalar_column]);
        }
    }
}

template<typename ValueType>
void require_mat_near_typed(
    const cv::Mat& actual,
    const cv::Mat& expected,
    double tolerance
) {
    const int scalar_columns = actual.cols * actual.channels();

    for (int row = 0; row < actual.rows; ++row) {
        const auto* actual_row = actual.ptr<ValueType>(row);
        const auto* expected_row = expected.ptr<ValueType>(row);

        for (int scalar_column = 0; scalar_column < scalar_columns; ++scalar_column) {
            const int column = scalar_column / actual.channels();
            const int channel = scalar_column % actual.channels();
            const double actual_value = static_cast<double>(actual_row[scalar_column]);
            const double expected_value = static_cast<double>(expected_row[scalar_column]);
            const double difference = std::abs(actual_value - expected_value);

            INFO(
                "row=" << row
                << ", column=" << column
                << ", channel=" << channel
                << ", actual=" << actual_value
                << ", expected=" << expected_value
                << ", difference=" << difference
                << ", tolerance=" << tolerance
            );
            REQUIRE(difference <= tolerance);
        }
    }
}

} // namespace detail

/**
 * @brief Requires two integer OpenCV matrices to be exactly equal.
 *
 * @details The comparison validates dimensionality, shape, type, channels, and
 * every scalar value. Failure messages identify the row, column, and channel
 * of the first different value.
 *
 * @param actual Matrix produced by the code under test.
 * @param expected Matrix containing the expected integer values.
 *
 * @pre Both matrices must be two-dimensional and use an integer OpenCV depth.
 */
inline void require_mat_exact(
    const cv::Mat& actual,
    const cv::Mat& expected
) {
    INFO("actual size=" << actual.rows << "x" << actual.cols);
    INFO("expected size=" << expected.rows << "x" << expected.cols);
    INFO("actual type=" << actual.type() << ", expected type=" << expected.type());

    REQUIRE_FALSE(actual.empty());
    REQUIRE_FALSE(expected.empty());
    REQUIRE(actual.dims == 2);
    REQUIRE(expected.dims == 2);
    REQUIRE(actual.rows == expected.rows);
    REQUIRE(actual.cols == expected.cols);
    REQUIRE(actual.type() == expected.type());

    switch (actual.depth()) {
        case CV_8U:
            detail::require_mat_exact_typed<std::uint8_t>(actual, expected);
            break;
        case CV_8S:
            detail::require_mat_exact_typed<std::int8_t>(actual, expected);
            break;
        case CV_16U:
            detail::require_mat_exact_typed<std::uint16_t>(actual, expected);
            break;
        case CV_16S:
            detail::require_mat_exact_typed<std::int16_t>(actual, expected);
            break;
        case CV_32S:
            detail::require_mat_exact_typed<std::int32_t>(actual, expected);
            break;
        default:
            INFO("require_mat_exact supports only integer OpenCV matrix depths");
            REQUIRE(false);
    }
}

/**
 * @brief Requires two floating-point OpenCV matrices to be equal within a tolerance.
 *
 * @details The comparison validates dimensionality, shape, type, channels, and
 * every scalar value. Failure messages include coordinates, values, absolute
 * difference, and the accepted tolerance.
 *
 * @param actual Matrix produced by the code under test.
 * @param expected Matrix containing the expected floating-point values.
 * @param tolerance Maximum accepted absolute difference for each scalar value.
 *
 * @pre Both matrices must be two-dimensional and have depth CV_32F or CV_64F.
 */
inline void require_mat_near(
    const cv::Mat& actual,
    const cv::Mat& expected,
    double tolerance
) {
    INFO("actual size=" << actual.rows << "x" << actual.cols);
    INFO("expected size=" << expected.rows << "x" << expected.cols);
    INFO("actual type=" << actual.type() << ", expected type=" << expected.type());
    INFO("tolerance=" << tolerance);

    REQUIRE_FALSE(actual.empty());
    REQUIRE_FALSE(expected.empty());
    REQUIRE(actual.dims == 2);
    REQUIRE(expected.dims == 2);
    REQUIRE(actual.rows == expected.rows);
    REQUIRE(actual.cols == expected.cols);
    REQUIRE(actual.type() == expected.type());
    REQUIRE(std::isfinite(tolerance));
    REQUIRE(tolerance >= 0.0);

    switch (actual.depth()) {
        case CV_32F:
            detail::require_mat_near_typed<float>(actual, expected, tolerance);
            break;
        case CV_64F:
            detail::require_mat_near_typed<double>(actual, expected, tolerance);
            break;
        default:
            INFO("require_mat_near supports only CV_32F and CV_64F matrices");
            REQUIRE(false);
    }
}

} // namespace pdi::testing

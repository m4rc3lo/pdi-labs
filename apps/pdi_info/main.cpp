/**
 * @file main.cpp
 * @brief Prints configured project and OpenCV version information.
 */

#include "pdi/core/project_info.hpp"

#include <iostream>

int main() {
    std::cout
        << pdi::core::ProjectInfo::project_name()
        << ' '
        << pdi::core::ProjectInfo::project_version()
        << '\n'
        << "OpenCV "
        << pdi::core::ProjectInfo::opencv_version()
        << '\n';

    return 0;
}

#pragma once

#include <string_view>

namespace pdi::core {

/**
 * @brief Provides static metadata about the pdi-labs project.
 */
class ProjectInfo {
public:
    /**
     * @brief Returns the CMake project name.
     *
     * @return Project name with static storage duration.
     */
    [[nodiscard]] static std::string_view project_name() noexcept;

    /**
     * @brief Returns the configured semantic version.
     *
     * @return Project version with static storage duration.
     */
    [[nodiscard]] static std::string_view project_version() noexcept;

    /**
     * @brief Returns the OpenCV version used during compilation.
     *
     * @return OpenCV version with static storage duration.
     */
    [[nodiscard]] static std::string_view opencv_version() noexcept;
};

} // namespace pdi::core

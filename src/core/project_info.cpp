#include "pdi/core/project_info.hpp"

#include "pdi/version.hpp"

#include <opencv2/core/version.hpp>

namespace pdi::core {

std::string_view ProjectInfo::project_name() noexcept {
    return PDI_PROJECT_NAME;
}

std::string_view ProjectInfo::project_version() noexcept {
    return PDI_PROJECT_VERSION;
}

std::string_view ProjectInfo::opencv_version() noexcept {
    return CV_VERSION;
}

} // namespace pdi::core

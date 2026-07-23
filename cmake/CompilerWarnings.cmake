include_guard(GLOBAL)

function(pdi_enable_project_warnings target_name)
    if(MSVC)
        target_compile_options(
            "${target_name}"
            PRIVATE
                /W4
                /permissive-
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(
            "${target_name}"
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wshadow
                -Wconversion
                -Wsign-conversion
        )
    else()
        message(
            WARNING
            "No project warning profile is configured for "
            "${CMAKE_CXX_COMPILER_ID}."
        )
    endif()
endfunction()

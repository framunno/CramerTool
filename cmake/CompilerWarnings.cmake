# Warning flags for first-party targets only; Qt's own headers are included as
# system headers by the Qt CMake modules and are not affected.
function(cramertool_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8 /Zc:__cplusplus)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
    endif()

    if(CRAMERTOOL_WARNINGS_AS_ERRORS)
        set_target_properties(${target} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    endif()
endfunction()

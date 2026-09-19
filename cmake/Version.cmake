# Software version.
#
# The single source of truth is `project(CramerTool VERSION x.y.z)` in the root
# CMakeLists.txt. CMake needs plain integers there; releases, Git tags, the UI
# and the documentation use the zero-padded form MM.mm.pp (1.1.0 -> 01.01.00).
#
# Produces:
#   CRAMERTOOL_VERSION_STRING         e.g. "01.01.00"
#   <build>/generated/app/Version.hpp constants for the C++ code
#   <build>/generated/cramertool.rc   Windows resource with icon and version info

function(cramertool_pad2 out value)
    if(value LESS 10)
        set(${out} "0${value}" PARENT_SCOPE)
    else()
        set(${out} "${value}" PARENT_SCOPE)
    endif()
endfunction()

cramertool_pad2(CRAMERTOOL_VERSION_MAJOR_PADDED ${PROJECT_VERSION_MAJOR})
cramertool_pad2(CRAMERTOOL_VERSION_MINOR_PADDED ${PROJECT_VERSION_MINOR})
cramertool_pad2(CRAMERTOOL_VERSION_PATCH_PADDED ${PROJECT_VERSION_PATCH})
set(CRAMERTOOL_VERSION_STRING
    "${CRAMERTOOL_VERSION_MAJOR_PADDED}.${CRAMERTOOL_VERSION_MINOR_PADDED}.${CRAMERTOOL_VERSION_PATCH_PADDED}")

string(TIMESTAMP CRAMERTOOL_BUILD_YEAR "%Y")

set(CRAMERTOOL_GENERATED_DIR ${PROJECT_BINARY_DIR}/generated)
configure_file(${PROJECT_SOURCE_DIR}/src/app/Version.hpp.in ${CRAMERTOOL_GENERATED_DIR}/app/Version.hpp @ONLY)
if(WIN32)
    configure_file(${PROJECT_SOURCE_DIR}/resources/cramertool.rc.in ${CRAMERTOOL_GENERATED_DIR}/cramertool.rc @ONLY)
endif()

message(STATUS "CramerTool version ${CRAMERTOOL_VERSION_STRING}")

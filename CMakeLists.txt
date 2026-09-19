cmake_minimum_required(VERSION 3.21)

project(CramerTool
    VERSION 1.1.0  # shown as 01.01.00, see cmake/Version.cmake
    DESCRIPTION "Solves 3x3 linear systems with Cramer's rule"
    HOMEPAGE_URL "https://github.com/framunno/CramerTool"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CRAMERTOOL_BUILD_GUI          "Build the Qt desktop application"  ON)
option(CRAMERTOOL_BUILD_TESTS        "Build the unit tests"              ON)
option(CRAMERTOOL_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmake/Version.cmake)
include(cmake/CompilerWarnings.cmake)

# ---------------------------------------------------------------------------
# Core: determinants, Cramer's rule, rank analysis.
# Plain C++20, no Qt, so it can be unit tested and reused anywhere.
# ---------------------------------------------------------------------------
add_library(cramer_core STATIC
    src/core/CramerSolver.cpp
    src/core/CramerSolver.hpp
    src/core/LinearSystem.cpp
    src/core/LinearSystem.hpp
)
target_include_directories(cramer_core PUBLIC src)
target_compile_features(cramer_core PUBLIC cxx_std_20)
cramertool_set_warnings(cramer_core)

# ---------------------------------------------------------------------------
# Qt: needed by the application (Widgets) and by the tests (QTest).
# ---------------------------------------------------------------------------
if(CRAMERTOOL_BUILD_GUI OR CRAMERTOOL_BUILD_TESTS)
    set(_qt_components)
    if(CRAMERTOOL_BUILD_GUI)
        list(APPEND _qt_components Widgets LinguistTools)
    endif()
    if(CRAMERTOOL_BUILD_TESTS)
        list(APPEND _qt_components Test)
    endif()
    find_package(Qt6 6.2 REQUIRED COMPONENTS ${_qt_components})
    qt_standard_project_setup()  # AUTOMOC/AUTOUIC/AUTORCC and sane defaults
endif()

# ---------------------------------------------------------------------------
# Desktop application
# ---------------------------------------------------------------------------
if(CRAMERTOOL_BUILD_GUI)
    qt_add_executable(CramerTool
        src/app/CommandLine.cpp
        src/app/CommandLine.hpp
        src/app/main.cpp
        src/ui/MainWindow.cpp
        src/ui/MainWindow.hpp
        ${CRAMERTOOL_GENERATED_DIR}/app/Version.hpp
    )
    if(WIN32)
        # Icon and version information shown by Explorer.
        target_sources(CramerTool PRIVATE ${CRAMERTOOL_GENERATED_DIR}/cramertool.rc)
    endif()
    target_link_libraries(CramerTool PRIVATE cramer_core Qt6::Widgets)
    target_include_directories(CramerTool PRIVATE ${CRAMERTOOL_GENERATED_DIR})
    cramertool_set_warnings(CramerTool)

    # Window icon, embedded in the executable through the Qt resource system.
    qt_add_resources(CramerTool "assets"
        PREFIX "/icons"
        BASE resources
        FILES resources/cramertool.png
    )

    # Translations: `.ts` files are updated by the generated update_translations
    # target and compiled into the binary as :/i18n/CramerTool_<locale>.qm.
    qt_add_translations(CramerTool
        TS_FILES
            i18n/CramerTool_en_US.ts
            i18n/CramerTool_it_IT.ts
        RESOURCE_PREFIX "/i18n"
        LUPDATE_OPTIONS -no-obsolete
    )

    set_target_properties(CramerTool PROPERTIES
        WIN32_EXECUTABLE TRUE  # no console window on Windows
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CramerTool)
endif()

# ---------------------------------------------------------------------------
# Tests (QTest + CTest)
# ---------------------------------------------------------------------------
if(CRAMERTOOL_BUILD_TESTS)
    enable_testing()

    add_executable(cramer_tests tests/CramerSolverTests.cpp)
    target_link_libraries(cramer_tests PRIVATE cramer_core Qt6::Test)
    cramertool_set_warnings(cramer_tests)

    # "-o -,txt" makes QTest write its report to stdout, so a failure in CI shows
    # which test broke instead of just a non-zero exit code.
    add_test(NAME cramer_tests COMMAND cramer_tests -o "-,txt")
    if(WIN32)
        # CTest must find the Qt DLLs when it runs the executable.
        set_tests_properties(cramer_tests PROPERTIES
            ENVIRONMENT "PATH=$<TARGET_FILE_DIR:Qt6::Test>;$ENV{PATH}"
        )
    endif()
endif()

# ---------------------------------------------------------------------------
# Install and packaging: `cpack -C Release` produces a ZIP containing the
# executable together with the Qt runtime, ready to run on a clean machine.
# ---------------------------------------------------------------------------
if(CRAMERTOOL_BUILD_GUI)
    # The deployment script installs the Qt runtime into CMAKE_INSTALL_BINDIR,
    # so the executable has to live there too: side by side, or it would not
    # find its own libraries.
    install(TARGETS CramerTool
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION .
    )
    install(FILES LICENSE THIRD_PARTY_NOTICES.md README.md DESTINATION .)

    # windeployqt (or its platform equivalent) copies the Qt libraries and plugins.
    # --no-opengl-sw drops the 20 MB software OpenGL rasterizer, which a widgets
    # application never uses: Qt paints widgets with its own raster engine.
    qt_generate_deploy_app_script(
        TARGET CramerTool
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
        DEPLOY_TOOL_OPTIONS --no-opengl-sw
    )
    # QT_DEPLOY_BIN_DIR is read while the install script runs, and defaults to
    # "bin": that would leave the Qt libraries one directory away from the
    # executable, which then would not start.
    install(CODE "set(QT_DEPLOY_BIN_DIR \".\")")
    install(SCRIPT ${deploy_script})

    set(CPACK_GENERATOR ZIP)
    set(CPACK_PACKAGE_NAME CramerTool)
    set(CPACK_PACKAGE_VENDOR "Francesco Ramunno")
    set(CPACK_PACKAGE_VERSION "${CRAMERTOOL_VERSION_STRING}")
    set(CPACK_PACKAGE_FILE_NAME "CramerTool-${CRAMERTOOL_VERSION_STRING}-win64")
    set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
    include(CPack)
endif()

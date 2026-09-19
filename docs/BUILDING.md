# Building CramerTool

## 1. Requirements

| Requirement | Notes |
|---|---|
| **Qt 6.2 or newer** | Modules: Widgets, LinguistTools, Test. The official [online installer](https://www.qt.io/download-qt-installer) provides all of them. |
| **CMake 3.21 or newer** | Bundled with Qt (`Tools/CMake_64`) and with Visual Studio. |
| **A C++20 compiler** | MinGW 13 (shipped with Qt), MSVC 2019 16.11+, GCC 11+, Clang 14+. |

Nothing else is downloaded at configure time: Qt is the only dependency.

Verified on Windows 11 with Qt 6.11.1 and the MinGW 13.1 kit that Qt installs.

## 2. Command line

```bash
git clone https://github.com/framunno/CramerTool.git
cd CramerTool

# CMAKE_PREFIX_PATH points at the Qt kit you want to build against
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.11.1/mingw_64"
cmake --build build
ctest --test-dir build --output-on-failure

# run it (Windows: the Qt bin folder must be on PATH, or use the package below)
./build/CramerTool
```

With the Qt MinGW kit on Windows, the tools live in predictable places:

```powershell
$env:Path = "C:\Qt\6.11.1\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\CMake_64\bin;$env:Path"
cmake -S . -B build/mingw -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.11.1/mingw_64"
cmake --build build/mingw
```

### Presets

`CMakePresets.json` defines ready-made configurations. They expect the
environment variable `QT_DIR` to point at the kit, for example
`C:/Qt/6.11.1/mingw_64` or `/usr/lib/x86_64-linux-gnu/cmake/Qt6`:

```bash
cmake --preset ninja-release
cmake --build --preset ninja-release
ctest --preset ninja-release
```

| Preset | Generator | Notes |
|---|---|---|
| `ninja-debug` / `ninja-release` | Ninja | Any compiler, fastest turnaround |
| `vs2022` | Visual Studio 17 2022 | Build with `--config Debug\|Release` |

## 3. Qt Creator

*File ▸ Open File or Project…* and select `CMakeLists.txt`. Pick a kit, press
**Ctrl+R** to run or **Ctrl+Shift+B** to build. Tests appear in the *Tests* pane.

## 4. Visual Studio

*File ▸ Open ▸ Folder…* on the repository root. Add the Qt location to the CMake
settings (`CMAKE_PREFIX_PATH`) or configure from the command line first;
`CramerTool` is already the startup project.

## 5. Tests

```bash
ctest --test-dir build --output-on-failure
```

15 QTest cases cover the determinants, the three kinds of system, the rank
computation and the numerical tolerance. The report is printed by CTest, so a
failure names the test that broke.

To run the executable directly, with more detail:

```bash
./build/cramer_tests -v2
```

## 6. Release package

```bash
cmake --build build --config Release
cpack --config build/CPackConfig.cmake -C Release
```

The result is `build/dist/CramerTool-01.01.00-win64.zip`, containing the
executable, the Qt libraries and plugins it needs (collected by `windeployqt`
through `qt_generate_deploy_app_script`), the license and the third-party
notices. It runs on a machine without Qt installed.

## 7. Options

| Option | Default | Effect |
|---|---|---|
| `CRAMERTOOL_BUILD_GUI` | `ON` | Build the Qt application. |
| `CRAMERTOOL_BUILD_TESTS` | `ON` | Build the QTest suite. |
| `CRAMERTOOL_WARNINGS_AS_ERRORS` | `OFF` | Turn warnings into errors (used by CI). |

The solver alone, without the GUI, still needs Qt for the tests; to build just
the library:

```bash
cmake -S . -B build -DCRAMERTOOL_BUILD_GUI=OFF -DCRAMERTOOL_BUILD_TESTS=OFF
cmake --build build --target cramer_core
```

## 8. Translations

The `.ts` files in `i18n/` are compiled into the executable. After changing or
adding translatable strings:

```bash
cmake --build build --target update_translations   # runs lupdate
# translate with Qt Linguist, then rebuild
```

## 9. Troubleshooting

| Symptom | Cause and fix |
|---|---|
| `Could NOT find Qt6` | `CMAKE_PREFIX_PATH` does not point at a Qt kit. Pass the directory that contains `lib/cmake/Qt6`. |
| The app starts and immediately exits, or complains about `Qt6Widgets.dll` | The Qt `bin` folder is not on `PATH`. Run from Qt Creator, add it to `PATH`, or use the packaged ZIP. |
| `qt_standard_project_setup` unknown | Qt older than 6.2; upgrade or build with `-DCRAMERTOOL_BUILD_GUI=OFF`. |
| Tests fail to start on Windows | They need the Qt DLLs too; CTest adds them to `PATH` automatically, so run tests through `ctest`. |

# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/); versions use the
zero-padded `MM.mm.pp` form, matching the Git tags (`v01.01.00`) and the About
dialog.

## [01.01.00] — 2026-09-20

A refactoring of the whole program, plus the documentation it was missing.

### Added

- Classification of singular systems: *infinitely many solutions* or *no
  solution*, decided with the Rouché-Capelli theorem (rank of the coefficient
  matrix against rank of the augmented matrix) instead of the shortcut that
  compares Dx, Dy and Dz against zero — a shortcut that reports a contradictory
  system as indeterminate.
- The determinants D, Dx, Dy and Dz are shown in the window.
- Input validation: fields accept plain and scientific notation, invalid or
  empty entries are highlighted and explained.
- Menus and shortcuts: load example (Ctrl+E), clear (Ctrl+L), copy result
  (Ctrl+C), about, about Qt; Enter solves from any field.
- Command line: `--system="a,b,c,d; e,f,g,h; i,j,k,l"` and `--solve`, useful for
  demonstrations and reproducible screenshots.
- 15 unit tests (QTest + CTest) covering determinants, the three kinds of
  system, ranks and the behaviour with very large and very small coefficients.
- Italian translation alongside the English one.
- Application icon and Windows version information.
- Documentation: README with screenshots, build guide, architecture, and the
  mathematics behind the tool; changelog; third-party notices for Qt's LGPL;
  MIT license file.
- GitHub Actions workflow: build and test on Windows and Linux; release workflow
  that publishes a packaged ZIP on a tag.
- `cpack` package that bundles the Qt runtime, so the ZIP runs on a machine
  without Qt.

### Changed

- The program is split into layers: `src/core` (mathematics, no Qt),
  `src/ui` (the window), `src/app` (entry point). The solver is now testable on
  its own.
- The window is built from layouts instead of fixed coordinates, so it resizes
  and adapts to DPI scaling and to translations.
- Reading the fields no longer repeats twelve `if` blocks; every field is read
  and every error reported in one pass.
- CMake: options, single source of truth for the version, generated version
  header and Windows resource, warnings enabled (`/W4`, `-Wall -Wextra
  -Wpedantic -Wshadow -Wconversion`), install rules and packaging.

### Fixed

- Comparing the determinant against zero with `!=` on a `double`: the comparison
  now uses a tolerance that scales with the magnitude of the coefficients, so
  systems written in millions or millionths are classified correctly.
- Empty or non-numeric input used to produce the answer "IMP", the same message
  used for an impossible system; the two cases are now distinct.

## [01.00.00] — 2024

First release: a Qt window that solves a 3×3 system with Cramer's rule and shows
x, y and z, or "IMP" when the determinant is zero.

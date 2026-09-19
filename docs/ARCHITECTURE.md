# Architecture

CramerTool is a small application, so the interesting decision is where the line
between mathematics and interface is drawn.

## 1. Layers

```
┌─────────────────────────────────────────────────────────────┐
│ src/app      main(), command line (QCommandLineParser),      │
│              translations, generated version header          │
├─────────────────────────────────────────────────────────────┤
│ src/ui       MainWindow: fields, validation, formatting,     │
│              menus, clipboard, status messages               │
├─────────────────────────────────────────────────────────────┤
│ src/core     LinearSystem, determinant, Cramer's rule,       │
│              rank analysis, numerical tolerance              │
│              plain C++20 - no Qt, no I/O, no globals         │
└─────────────────────────────────────────────────────────────┘
```

Dependencies point downwards only. `src/core` does not include a single Qt
header: it can be unit tested without a GUI, reused in a console tool or a web
service, and compiled by any C++20 compiler.

The UI never computes anything. It reads text, converts it to numbers, hands a
`LinearSystem` to the solver, and renders the `Solution` it gets back. That is
why the tests can cover the whole of the interesting behaviour.

## 2. Data flow

```
 QLineEdit ×12 ──readSystem()──▶ LinearSystem ──cramer::solve()──▶ Solution
                     │                                                 │
          invalid field is highlighted                    values, type, D, Dx, Dy, Dz
                                                                       │
                                                       showSolution() ─┴─▶ labels + status bar
```

`readSystem()` reads every field even after the first failure, so the user sees
all the invalid entries at once instead of fixing them one per attempt.

## 3. The core API

```cpp
struct Matrix3    { std::array<std::array<double, 3>, 3> rows; };
struct Vector3    { std::array<double, 3> values; };
struct LinearSystem { Matrix3 coefficients; Vector3 constants; };

enum class SolutionType : std::uint8_t { Unique, Infinite, Inconsistent };

struct Solution {
    SolutionType type;
    Vector3 values;       // meaningful when type == Unique
    double determinant;   // D
    Vector3 numerators;   // Dx, Dy, Dz
};

Solution solve(const LinearSystem&);
int rank(const Matrix3&);       // 0..3
int rank(const LinearSystem&);  // rank of [A|b]
```

Two details worth pointing out:

- **The determinants are part of the result.** They are what makes the tool
  useful for studying the method, and returning them costs nothing since they
  are computed anyway.
- **`SolutionType` has three values, not two.** Reporting "impossible" for both
  a contradictory system and one with infinitely many solutions — as the first
  version of this program did — throws away the more interesting half of the
  answer.

## 4. Design decisions

**Widgets in code, not in a `.ui` file.** For a form of this size, building the
layout in `MainWindow.cpp` keeps everything in one reviewable place, is checked
by the compiler, and produces diffs that can be read. Qt Designer earns its keep
on larger, more visual layouts.

**Layouts instead of fixed geometry.** The original version placed widgets at
absolute coordinates and called `setFixedSize()`. Grid and box layouts adapt to
font size, DPI scaling and translations, all of which change the size of labels.

**Validation at the edge.** `QDoubleValidator` in the C locale keeps most
nonsense out of the fields, and `readSystem()` re-checks before converting. The
core assumes it gets numbers; it never parses text.

**Tolerances live in the core**, next to the arithmetic they protect, and scale
with the magnitude of the input (see [MATH.md](MATH.md)).

**No global state.** The old implementation kept the matrix, the determinants
and the results as members of the window, re-used across clicks. Now each click
builds a `LinearSystem`, gets a `Solution`, and the window only remembers the
last one for the clipboard.

## 5. What changed in 01.01.00

| Before | Now |
|---|---|
| Whole program in `mainwindow.cpp`, UI and maths mixed | `core` / `ui` / `app` layers, maths independently testable |
| `QTextEdit` fields at absolute coordinates, fixed window size | `QLineEdit` with validators inside layouts, resizable window |
| Twelve repeated `if (ret)` blocks to read the fields | one loop plus a helper, every error reported at once |
| Empty or invalid input produced the answer "IMP" | invalid fields highlighted, with an explanatory message |
| `det != 0` compared a double for exact equality | tolerance scaled to the size of the coefficients |
| D = 0 reported as "IMP" in every case | Rouché-Capelli: *no solution* or *infinitely many* |
| Determinants computed and discarded | D, Dx, Dy, Dz shown in the window |
| No tests | 15 QTest cases, run by CTest |
| Qt Creator's default CMakeLists | options, version generation, translations, install, CPack, CI |

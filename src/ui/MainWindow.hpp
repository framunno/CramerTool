#pragma once

#include "core/CramerSolver.hpp"

#include <QMainWindow>

#include <array>
#include <optional>

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace cramer::ui
{

/// The application window: three equations in, a verdict and the determinants out.
///
/// The widgets are created in code rather than in a .ui file. For a form this
/// size it keeps everything in one reviewable place, it is checked by the
/// compiler, and it produces readable diffs; Qt Designer is the better choice
/// once a layout grows large enough to be worth editing visually.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    /// Fills the form with `system`, as the command line and the Example button do.
    void setSystem(const LinearSystem& system);

    /// Solves whatever the form currently contains.
    void solve();

private:
    // Construction, one piece of the window per function.
    void buildMenus();
    QGroupBox* buildSystemGroup();
    QGroupBox* buildSolutionGroup();
    QGroupBox* buildDeterminantGroup();
    QWidget* buildButtonRow();

    // Actions.
    void clearAll();
    void loadExample();
    void copyResult();
    void showAbout();

    /// Reads the twelve fields. Returns nothing and marks the offending field
    /// when a value is missing or not a number.
    std::optional<LinearSystem> readSystem();

    void showSolution(const Solution& solution);
    void clearOutputs();

    std::array<std::array<QLineEdit*, 3>, 3> coefficients_{};  ///< [row][x, y, z]
    std::array<QLineEdit*, 3> constants_{};                    ///< right-hand side
    std::array<QLabel*, 3> solutionValues_{};                  ///< x, y, z
    std::array<QLabel*, 4> determinants_{};                    ///< D, Dx, Dy, Dz
    QLabel* verdict_ = nullptr;
    QPushButton* copyButton_ = nullptr;

    std::optional<Solution> lastSolution_;
};

}  // namespace cramer::ui

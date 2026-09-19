#include "ui/MainWindow.hpp"

#include "app/Version.hpp"

#include <QApplication>
#include <QClipboard>
#include <QDoubleValidator>
#include <QFont>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>

namespace cramer::ui
{

namespace
{

/// Numbers are shown with up to 10 significant digits, then trimmed: 2 stays
/// "2" instead of "2.0000000000", while 1/3 keeps its digits.
QString formatNumber(double value)
{
    return QString::number(value, 'g', 10);
}

/// One coefficient or constant field. Accepts plain and scientific notation in
/// the C locale, so "1.5" and "1.5e-3" both work regardless of system settings.
QLineEdit* createNumberField(const QString& placeholder)
{
    auto* field = new QLineEdit;
    field->setAlignment(Qt::AlignRight);
    field->setPlaceholderText(placeholder);
    field->setClearButtonEnabled(true);
    field->setMaximumWidth(110);

    auto* validator = new QDoubleValidator(field);
    validator->setNotation(QDoubleValidator::ScientificNotation);
    validator->setLocale(QLocale::c());
    field->setValidator(validator);
    return field;
}

QLabel* createTermLabel(const QString& text)
{
    auto* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

/// Bold, slightly larger label used for results.
QLabel* createValueLabel()
{
    auto* label = new QLabel(QStringLiteral("—"));
    QFont font = label->font();
    font.setBold(true);
    font.setPointSizeF(font.pointSizeF() + 1.0);
    label->setFont(font);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    return label;
}

}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("CramerTool %1").arg(version::kString));

    buildMenus();

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(14);
    layout->addWidget(buildSystemGroup());
    layout->addWidget(buildButtonRow());
    layout->addWidget(buildSolutionGroup());
    layout->addWidget(buildDeterminantGroup());
    layout->addStretch();
    setCentralWidget(central);

    statusBar()->showMessage(tr("Enter the coefficients and press Solve."));
    coefficients_[0][0]->setFocus();
}

void MainWindow::buildMenus()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("Load &example"), QKeySequence(Qt::CTRL | Qt::Key_E), this, &MainWindow::loadExample);
    fileMenu->addAction(tr("&Clear"), QKeySequence(Qt::CTRL | Qt::Key_L), this, &MainWindow::clearAll);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), QKeySequence::Quit, this, &QWidget::close);

    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Copy result"), QKeySequence::Copy, this, &MainWindow::copyResult);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About CramerTool"), this, &MainWindow::showAbout);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}

QGroupBox* MainWindow::buildSystemGroup()
{
    auto* group = new QGroupBox(tr("System of equations"));
    auto* grid = new QGridLayout(group);
    grid->setHorizontalSpacing(8);
    grid->setVerticalSpacing(10);

    // Each row reads:  [a] x + [b] y + [c] z = [d]
    static constexpr std::array<const char*, 3> kUnknowns = {"x", "y", "z"};
    for (int row = 0; row < 3; ++row)
    {
        int column = 0;
        for (int unknown = 0; unknown < 3; ++unknown)
        {
            QLineEdit* field = createNumberField(QStringLiteral("0"));
            field->setToolTip(tr("Coefficient of %1 in equation %2")
                                  .arg(QString::fromLatin1(kUnknowns[static_cast<std::size_t>(unknown)]))
                                  .arg(row + 1));
            coefficients_[static_cast<std::size_t>(row)][static_cast<std::size_t>(unknown)] = field;

            grid->addWidget(field, row, column++);
            grid->addWidget(createTermLabel(QString::fromLatin1(kUnknowns[static_cast<std::size_t>(unknown)])), row,
                            column++);
            if (unknown < 2)
            {
                grid->addWidget(createTermLabel(QStringLiteral("+")), row, column++);
            }
        }
        grid->addWidget(createTermLabel(QStringLiteral("=")), row, column++);

        QLineEdit* constant = createNumberField(QStringLiteral("0"));
        constant->setToolTip(tr("Constant term of equation %1").arg(row + 1));
        constants_[static_cast<std::size_t>(row)] = constant;
        grid->addWidget(constant, row, column);
    }
    grid->setColumnStretch(grid->columnCount(), 1);

    // Enter anywhere in the form solves the system.
    for (auto& row : coefficients_)
    {
        for (QLineEdit* field : row)
        {
            connect(field, &QLineEdit::returnPressed, this, &MainWindow::solve);
        }
    }
    for (QLineEdit* field : constants_)
    {
        connect(field, &QLineEdit::returnPressed, this, &MainWindow::solve);
    }
    return group;
}

QWidget* MainWindow::buildButtonRow()
{
    auto* row = new QWidget;
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* solveButton = new QPushButton(tr("&Solve"));
    solveButton->setDefault(true);  // Enter activates it
    solveButton->setObjectName(QStringLiteral("solveButton"));
    connect(solveButton, &QPushButton::clicked, this, &MainWindow::solve);

    auto* clearButton = new QPushButton(tr("C&lear"));
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearAll);

    auto* exampleButton = new QPushButton(tr("&Example"));
    exampleButton->setToolTip(tr("Fill the form with a system that has a unique solution"));
    connect(exampleButton, &QPushButton::clicked, this, &MainWindow::loadExample);

    copyButton_ = new QPushButton(tr("&Copy result"));
    copyButton_->setEnabled(false);
    connect(copyButton_, &QPushButton::clicked, this, &MainWindow::copyResult);

    layout->addWidget(solveButton);
    layout->addWidget(clearButton);
    layout->addWidget(exampleButton);
    layout->addStretch();
    layout->addWidget(copyButton_);
    return row;
}

QGroupBox* MainWindow::buildSolutionGroup()
{
    auto* group = new QGroupBox(tr("Solution"));
    auto* layout = new QVBoxLayout(group);

    verdict_ = new QLabel(tr("Not solved yet."));
    verdict_->setWordWrap(true);
    layout->addWidget(verdict_);

    auto* values = new QHBoxLayout;
    static constexpr std::array<const char*, 3> kUnknowns = {"x", "y", "z"};
    for (std::size_t i = 0; i < solutionValues_.size(); ++i)
    {
        solutionValues_[i] = createValueLabel();
        values->addWidget(new QLabel(tr("%1 =").arg(QString::fromLatin1(kUnknowns[i]))));
        values->addWidget(solutionValues_[i]);
        values->addSpacing(24);
    }
    values->addStretch();
    layout->addLayout(values);
    return group;
}

QGroupBox* MainWindow::buildDeterminantGroup()
{
    auto* group = new QGroupBox(tr("Determinants"));
    group->setToolTip(tr("The intermediate values of Cramer's rule: x = Dx/D, y = Dy/D, z = Dz/D"));
    auto* layout = new QHBoxLayout(group);

    static constexpr std::array<const char*, 4> kNames = {"D", "Dx", "Dy", "Dz"};
    for (std::size_t i = 0; i < determinants_.size(); ++i)
    {
        determinants_[i] = createValueLabel();
        layout->addWidget(new QLabel(tr("%1 =").arg(QString::fromLatin1(kNames[i]))));
        layout->addWidget(determinants_[i]);
        layout->addSpacing(20);
    }
    layout->addStretch();
    return group;
}

std::optional<LinearSystem> MainWindow::readSystem()
{
    LinearSystem system;
    const QString invalidStyle = QStringLiteral("background-color: #ffd7d7;");

    auto readField = [&](QLineEdit* field, double& target)
    {
        bool ok = false;
        const double value = field->text().trimmed().toDouble(&ok);
        field->setStyleSheet(ok ? QString() : invalidStyle);
        if (ok)
        {
            target = value;
        }
        return ok;
    };

    bool allValid = true;
    for (std::size_t row = 0; row < 3; ++row)
    {
        for (std::size_t column = 0; column < 3; ++column)
        {
            // Every field is read even after a failure, so the user sees all the
            // problems at once instead of one per attempt.
            allValid &= readField(coefficients_[row][column], system.coefficients.at(row, column));
        }
        allValid &= readField(constants_[row], system.constants[row]);
    }
    if (!allValid)
    {
        return std::nullopt;
    }
    return system;
}

void MainWindow::solve()
{
    const std::optional<LinearSystem> system = readSystem();
    if (!system)
    {
        clearOutputs();
        verdict_->setText(tr("Some fields are empty or not valid numbers (highlighted in red)."));
        statusBar()->showMessage(tr("Nothing to solve: fix the highlighted fields."), 5000);
        return;
    }

    const Solution solution = cramer::solve(*system);
    lastSolution_ = solution;
    showSolution(solution);
}

void MainWindow::showSolution(const Solution& solution)
{
    for (std::size_t i = 0; i < determinants_.size(); ++i)
    {
        const double value = i == 0 ? solution.determinant : solution.numerators[i - 1];
        determinants_[i]->setText(formatNumber(value));
    }

    switch (solution.type)
    {
    case SolutionType::Unique:
        for (std::size_t i = 0; i < solutionValues_.size(); ++i)
        {
            solutionValues_[i]->setText(formatNumber(solution.values[i]));
        }
        verdict_->setText(tr("Unique solution: the three planes meet in one point (D ≠ 0)."));
        statusBar()->showMessage(tr("Solved."), 4000);
        break;

    case SolutionType::Infinite:
        for (QLabel* label : solutionValues_)
        {
            label->setText(QStringLiteral("∞"));
        }
        verdict_->setText(tr("Infinitely many solutions: D = 0 and the equations are consistent, so they share a "
                             "line or a plane."));
        statusBar()->showMessage(tr("Indeterminate system."), 4000);
        break;

    case SolutionType::Inconsistent:
        for (QLabel* label : solutionValues_)
        {
            label->setText(QStringLiteral("—"));
        }
        verdict_->setText(tr("No solution: D = 0 and the equations contradict each other."));
        statusBar()->showMessage(tr("Impossible system."), 4000);
        break;
    }
    copyButton_->setEnabled(true);
}

void MainWindow::clearOutputs()
{
    for (QLabel* label : solutionValues_)
    {
        label->setText(QStringLiteral("—"));
    }
    for (QLabel* label : determinants_)
    {
        label->setText(QStringLiteral("—"));
    }
    lastSolution_.reset();
    copyButton_->setEnabled(false);
}

void MainWindow::clearAll()
{
    for (auto& row : coefficients_)
    {
        for (QLineEdit* field : row)
        {
            field->clear();
            field->setStyleSheet(QString());
        }
    }
    for (QLineEdit* field : constants_)
    {
        field->clear();
        field->setStyleSheet(QString());
    }
    clearOutputs();
    verdict_->setText(tr("Not solved yet."));
    statusBar()->showMessage(tr("Cleared."), 3000);
    coefficients_[0][0]->setFocus();
}

void MainWindow::setSystem(const LinearSystem& system)
{
    for (std::size_t row = 0; row < 3; ++row)
    {
        for (std::size_t column = 0; column < 3; ++column)
        {
            coefficients_[row][column]->setText(formatNumber(system.coefficients.at(row, column)));
            coefficients_[row][column]->setStyleSheet(QString());
        }
        constants_[row]->setText(formatNumber(system.constants[row]));
        constants_[row]->setStyleSheet(QString());
    }
}

void MainWindow::loadExample()
{
    //  2x +  y -  z =   8
    // -3x -  y + 2z = -11    ->  x = 2, y = 3, z = -1
    // -2x +  y + 2z =  -3
    static const LinearSystem example{
        .coefficients = {{{{2, 1, -1}, {-3, -1, 2}, {-2, 1, 2}}}},
        .constants = {{8, -11, -3}},
    };

    setSystem(example);
    statusBar()->showMessage(tr("Example loaded: the solution is x = 2, y = 3, z = -1."), 6000);
}

void MainWindow::copyResult()
{
    if (!lastSolution_)
    {
        return;
    }

    QString text;
    switch (lastSolution_->type)
    {
    case SolutionType::Unique:
        text = tr("x = %1\ny = %2\nz = %3\n")
                   .arg(formatNumber(lastSolution_->values.x()), formatNumber(lastSolution_->values.y()),
                        formatNumber(lastSolution_->values.z()));
        break;
    case SolutionType::Infinite:
        text = tr("Infinitely many solutions\n");
        break;
    case SolutionType::Inconsistent:
        text = tr("No solution\n");
        break;
    }
    text += tr("D = %1, Dx = %2, Dy = %3, Dz = %4")
                .arg(formatNumber(lastSolution_->determinant), formatNumber(lastSolution_->numerators[0]),
                     formatNumber(lastSolution_->numerators[1]), formatNumber(lastSolution_->numerators[2]));

    QApplication::clipboard()->setText(text);
    statusBar()->showMessage(tr("Result copied to the clipboard."), 3000);
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, tr("About CramerTool"),
                       tr("<h3>CramerTool %1</h3>"
                          "<p>Solves systems of three linear equations in three unknowns with "
                          "Cramer's rule, and shows the determinants behind the result.</p>"
                          "<p>Written by Francesco Ramunno. Released under the MIT License.</p>"
                          "<p>Built with Qt %2, used under the LGPL v3. "
                          "See THIRD_PARTY_NOTICES.md for details.</p>")
                           .arg(QString::fromLatin1(version::kString), QString::fromLatin1(qVersion())));
}

}  // namespace cramer::ui

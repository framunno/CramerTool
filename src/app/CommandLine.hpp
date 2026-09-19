#pragma once

#include "core/LinearSystem.hpp"

#include <QString>

#include <optional>

class QCoreApplication;

namespace cramer::app
{

/// What the command line asked for.
///
///     CramerTool --system="2,1,-1,8; -3,-1,2,-11; -2,1,2,-3" --solve
///
/// Handy for demonstrations, for reproducible screenshots, and for checking a
/// system without clicking: the window opens with the fields already filled.
struct Options
{
    std::optional<LinearSystem> system;  ///< --system, when given and well formed
    bool solveImmediately = false;       ///< --solve
};

/// Parses the application arguments. Shows Qt's own help/version dialogs and
/// exits when --help or --version is given.
Options parseCommandLine(const QCoreApplication& application);

/// Parses "a,b,c,d; e,f,g,h; i,j,k,l" into a system. Whitespace is ignored.
/// Returns nothing when the text is malformed, so the caller can report it.
std::optional<LinearSystem> parseSystem(const QString& text);

}  // namespace cramer::app

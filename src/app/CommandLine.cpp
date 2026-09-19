#include "app/CommandLine.hpp"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QStringList>

namespace cramer::app
{

std::optional<LinearSystem> parseSystem(const QString& text)
{
    const QStringList rows = text.split(QLatin1Char(';'), Qt::SkipEmptyParts);
    if (rows.size() != 3)
    {
        return std::nullopt;
    }

    LinearSystem system;
    for (int row = 0; row < 3; ++row)
    {
        const QStringList values = rows.at(row).split(QLatin1Char(','), Qt::SkipEmptyParts);
        if (values.size() != 4)  // three coefficients plus the constant term
        {
            return std::nullopt;
        }
        for (int column = 0; column < 4; ++column)
        {
            bool ok = false;
            const double value = values.at(column).trimmed().toDouble(&ok);
            if (!ok)
            {
                return std::nullopt;
            }
            if (column < 3)
            {
                system.coefficients.at(static_cast<std::size_t>(row), static_cast<std::size_t>(column)) = value;
            }
            else
            {
                system.constants[static_cast<std::size_t>(row)] = value;
            }
        }
    }
    return system;
}

Options parseCommandLine(const QCoreApplication& application)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QCoreApplication::translate("CommandLine", "Solves 3x3 linear systems with Cramer's rule."));
    parser.addHelpOption();     // --help, handled and exits
    parser.addVersionOption();  // --version, handled and exits

    const QCommandLineOption systemOption(
        QStringList{QStringLiteral("s"), QStringLiteral("system")},
        QCoreApplication::translate("CommandLine", "Fill the form with a system, given as three comma-separated "
                                                   "rows of four numbers separated by semicolons."),
        QStringLiteral("\"a,b,c,d; e,f,g,h; i,j,k,l\""));
    const QCommandLineOption solveOption(
        QStringLiteral("solve"),
        QCoreApplication::translate("CommandLine", "Solve the system right away, without clicking Solve."));

    parser.addOption(systemOption);
    parser.addOption(solveOption);
    parser.process(application);

    Options options;
    if (parser.isSet(systemOption))
    {
        options.system = parseSystem(parser.value(systemOption));
    }
    options.solveImmediately = parser.isSet(solveOption);
    return options;
}

}  // namespace cramer::app

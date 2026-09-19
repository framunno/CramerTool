// CramerTool - solves 3x3 linear systems with Cramer's rule.
//
// Layout of the project:
//   src/core   the solver: determinants, Cramer's rule, rank analysis. No Qt.
//   src/ui     the Qt widgets that read the numbers and show the result.
//   src/app    this file: application metadata, translations, window.

#include "app/CommandLine.hpp"
#include "app/Version.hpp"
#include "ui/MainWindow.hpp"

#include <QApplication>
#include <QIcon>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QTranslator>

namespace
{

/// Installs the CramerTool translation matching the system language, plus Qt's
/// own translation for the strings that come from the framework (dialog
/// buttons, the About Qt window). Missing files are not an error: the
/// application then shows its original English strings.
void installTranslations(QApplication& app)
{
    static QTranslator appTranslator;
    static QTranslator qtTranslator;

    const QLocale locale = QLocale::system();
    if (appTranslator.load(locale, QStringLiteral("CramerTool"), QStringLiteral("_"), QStringLiteral(":/i18n")))
    {
        app.installTranslator(&appTranslator);
    }
    if (qtTranslator.load(locale, QStringLiteral("qtbase"), QStringLiteral("_"),
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        app.installTranslator(&qtTranslator);
    }
}

}  // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Used by Qt for settings paths and by the OS for window grouping.
    QApplication::setApplicationName(QStringLiteral("CramerTool"));
    QApplication::setApplicationVersion(QString::fromLatin1(cramer::version::kString));
    QApplication::setOrganizationName(QStringLiteral("Francesco Ramunno"));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/cramertool.png")));

    installTranslations(app);

    // Parsed after the translations are installed, so --help speaks the user's
    // language.
    const cramer::app::Options options = cramer::app::parseCommandLine(app);

    cramer::ui::MainWindow window;
    if (options.system)
    {
        window.setSystem(*options.system);
        if (options.solveImmediately)
        {
            window.solve();
        }
    }
    else if (options.solveImmediately)
    {
        QMessageBox::warning(nullptr, QApplication::applicationName(),
                             QObject::tr("--solve needs a valid --system=\"a,b,c,d; e,f,g,h; i,j,k,l\"."));
    }

    window.show();
    return QApplication::exec();
}

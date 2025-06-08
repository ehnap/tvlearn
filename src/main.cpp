#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    // Set application information
    QApplication::setApplicationName("HarperTV");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("HarperTV");
    QApplication::setOrganizationDomain("harpertv.example.com");

    // Create application
    QApplication app(argc, argv);

    // Load translations
    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(), "qt", "_",
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        app.installTranslator(&qtTranslator);
    }

    QTranslator appTranslator;
    if (appTranslator.load(QLocale::system(), "harpertv", "_", ":/translations"))
    {
        app.installTranslator(&appTranslator);
    }

    // Load style sheet
    QFile styleFile(":/styles/default.qss");
    if (styleFile.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }
    else
    {
        qWarning() << "Failed to load style sheet:" << styleFile.errorString();
    }

    // Create main window
    MainWindow mainWindow;

    // Initialize main window
    if (!mainWindow.initialize())
    {
        return 1;
    }

    // Show main window
    mainWindow.show();

    // Run application
    return app.exec();
}
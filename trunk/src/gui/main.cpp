#include <QtGui/QApplication>
#include "MainWindow.h"
#include "../G/GLogger.h"

//GLogger logg("lastrun.log");

int main(int argc, char *argv[])
{
    // To make QSettings work
    QCoreApplication::setOrganizationName("FL");
    QCoreApplication::setApplicationName("FLAnalyzer");

    // Create application and main form
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // Run
    return a.exec();
}

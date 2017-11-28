#include <QApplication>

#include "IterationInfo.h"
#include "MainWindow/MainWindow.h"

int main(int argv, char* args[])
{
    QApplication app(argv, args);

    QApplication::setOrganizationName("Genesis");
    QApplication::setOrganizationDomain("genesis.com");
    QApplication::setApplicationName("Evolution");

    qRegisterMetaType<IterationInfo>();

    MainWindow main_window;
    main_window.show();

    return app.exec();
}

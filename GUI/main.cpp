#include <QApplication>

#include "IterationInfo.h"
#include "MainWindow/MainWindow.h"

extern "C"
{
#include "PluginManager/PluginManager.h"
}

int main(int argv, char* args[])
{
    if (!load_plugins("../Plugins/Objectives", "../Plugins/Operators"))
    {
        qWarning("Failed to load plugins: %s", g_PM_last_error);
        return 1;
    }

    QApplication app(argv, args);

    QApplication::setOrganizationName("Genesis");
    QApplication::setOrganizationDomain("genesis.com");
    QApplication::setApplicationName("Evolution");

    qRegisterMetaType<IterationInfo>();

    MainWindow main_window;
    main_window.show();

    int status = app.exec();
    unload_plugins();
    return status;
}

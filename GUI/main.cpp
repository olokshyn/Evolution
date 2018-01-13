#include <cstring>

#include <memory>

#include <QApplication>

#include "IterationInfo.h"
#include "MainWindow/MainWindow.h"

extern "C"
{
#include "PluginManager/PluginManager.h"
}

int main(int argc, char* argv[])
{
    if (argc <= 1 || strncmp("-ns", argv[1], 3) != 0) {
        srand((unsigned int)time(NULL));
    }

    if (!load_plugins("../Plugins/Objectives", "../Plugins/Operators"))
    {
        qWarning("Failed to load plugins: %s", g_PM_last_error);
        return 1;
    }
    auto plugins_deleter = [](void*) { unload_plugins(); };
    // unload plugins only after app gets destructed
    std::unique_ptr<void, decltype(plugins_deleter)>
            plugins_guard(reinterpret_cast<void*>(1), plugins_deleter);

    QApplication app(argc, argv);

    QApplication::setOrganizationName("Genesis");
    QApplication::setOrganizationDomain("genesis.com");
    QApplication::setApplicationName("Evolution");

    qRegisterMetaType<IterationInfo>();

    MainWindow main_window;
    main_window.showMaximized();

    return app.exec();
}

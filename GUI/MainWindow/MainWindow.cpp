//
// Created by Oleg on 11/27/17.
//

#include "MainWindow.h"

#include <QTabWidget>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include "EvolutionWidget/EvolutionWidget.h"

namespace
{
    const QString g_tab_stylesheet =
            "background-image: url(:/images/genesis.jpg); "
                    "background-repeat: no-repeat;"
                    "background-position: center;";
}

MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent),
          m_tab(new QTabWidget(this)),
          m_launcher(m_tab)
{
    m_tab->setTabsClosable(true);
    m_tab->setStyleSheet(g_tab_stylesheet);

    connect(m_tab, &QTabWidget::tabCloseRequested,
            this, &MainWindow::close_tab);
    connect(m_tab, &QTabWidget::currentChanged,
            this, &MainWindow::current_tab_changed);

    setCentralWidget(m_tab);

    auto action = new QAction("Launch...", this);
    connect(action, &QAction::triggered, this,
            &MainWindow::launch_evolution);
    action->setShortcut(tr("CTRL+N"));

    auto evolution_menu = new QMenu(this);
    evolution_menu->setTitle("Evolution");
    evolution_menu->addAction(action);
    menuBar()->addMenu(evolution_menu);

    resize(800, 800);
}

void MainWindow::launch_evolution()
{
    m_launcher.show();
}

void MainWindow::close_tab(int index)
{
    QWidget* widget = m_tab->widget(index);
    if (!widget)
    {
        qWarning("Tab to remove is not found");
        return;
    }

    widget->deleteLater();
    m_tab->removeTab(index);
}

void MainWindow::current_tab_changed(int index)
{
    if (index == -1)
    {
        m_tab->setStyleSheet(g_tab_stylesheet);
    }
    else if (!m_tab->styleSheet().isEmpty())
    {
        m_tab->setStyleSheet("");
    }
}

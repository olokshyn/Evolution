//
// Created by Oleg on 11/27/17.
//

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>

#include "ToolsWidgets/LauncherWidget.h"

class QTabWidget;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

public slots:
    void launch_evolution();
    void close_tab(int index);
    void current_tab_changed(int index);

private:
    QTabWidget* m_tab;
    LauncherWidget m_launcher;
};

#endif //GUI_MAINWINDOW_H

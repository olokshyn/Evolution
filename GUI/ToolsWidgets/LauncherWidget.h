//
// Created by Oleg on 11/27/17.
//

#ifndef GUI_LAUNCHERWIDGET_H
#define GUI_LAUNCHERWIDGET_H

#include <QWidget>

class QTabWidget;
class QLineEdit;
class QPushButton;

class SettingsWidget;
class EvolutionWidget;

class LauncherWidget : public QWidget
{
Q_OBJECT

public:
    explicit LauncherWidget(QTabWidget* tab,
                            QWidget* parent = nullptr);

public slots:
    void launch_evolution();
    void cancel();

private:
    QTabWidget* m_tab;
    SettingsWidget* m_settings_widget;
    QLineEdit* m_name_edit;
    QPushButton* m_launch_btn;
    QPushButton* m_cancel_btn;
};


#endif //GUI_LAUNCHERWIDGET_H

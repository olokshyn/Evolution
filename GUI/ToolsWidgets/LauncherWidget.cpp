//
// Created by Oleg on 11/27/17.
//

#include "LauncherWidget.h"

#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

#include "SettingsWidget.h"
#include "EvolutionWidget/EvolutionWidget.h"

LauncherWidget::LauncherWidget(QTabWidget* tab, QWidget* parent)
        : QWidget(parent),
          m_tab(tab),
          m_settings_widget(new SettingsWidget(this)),
          m_name_edit(new QLineEdit(this)),
          m_launch_btn(new QPushButton("Launch", this)),
          m_cancel_btn(new QPushButton("Cancel", this))
{
    connect(m_launch_btn, &QPushButton::clicked,
            this, &LauncherWidget::launch_evolution);
    connect(m_cancel_btn, &QPushButton::clicked,
            this, &LauncherWidget::cancel);

    auto layout = new QGridLayout(this);
    layout->addWidget(m_settings_widget, 0, 0, 1, 4);

    layout->addWidget(new QLabel("Name", this), 1, 0);
    layout->addWidget(m_name_edit, 1, 1);

    layout->addWidget(m_cancel_btn, 1, 2);
    layout->addWidget(m_launch_btn, 1, 3);
}

void LauncherWidget::launch_evolution()
{
    if (m_name_edit->text().isEmpty())
    {
        QMessageBox message_box;
        message_box.setText("Please, fill in the Name field");
        message_box.exec();
        return;
    }

    m_settings_widget->save_settings();

    auto widget = new EvolutionWidget(*m_settings_widget,
                                      m_name_edit->text().toStdString(),
                                      m_tab);
    m_tab->addTab(widget, m_name_edit->text());
    widget->start_evolution();

    hide();
    m_name_edit->clear();
}

void LauncherWidget::cancel()
{
    hide();
    m_settings_widget->reset_settings();
    m_name_edit->clear();
}

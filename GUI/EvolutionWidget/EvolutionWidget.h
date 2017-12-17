//
// Created by Oleg on 11/19/17.
//

#ifndef GUI_EVOLUTIONWIDGET_H
#define GUI_EVOLUTIONWIDGET_H

#include <vector>
#include <string>
#include <thread>

#include <QWidget>
#include <QChartView>
#include <QLineSeries>
#include <QList>

extern "C"
{
#include "GeneticAlgorithm/GAParameters.h"
#include "GeneticAlgorithm/GAOperators.h"
}

#include "IterationInfo.h"
#include "ToolsWidgets/SettingsWidget.h"

class QLabel;
class QPushButton;

class EvolutionWorker;

class EvolutionWidget : public QWidget
{
Q_OBJECT

public:
    explicit EvolutionWidget(const SettingsWidget& settings,
                             const std::string& name,
                             QWidget* parent = nullptr);
    ~EvolutionWidget() override;

public slots:
    void start_evolution();
    void stop_evolution();
    void show_info();

private slots:
    void plot_iterations(const QList<IterationInfo>& infos);
    void optimum_reached(double optimum);

private:
    void add_evolution_serieses(QtCharts::QChartView* chart_view,
                                size_t max_generations_count,
                                size_t world_size);

private:
    QLabel* m_generation_number_lbl;
    QLabel* m_max_fitness_lbl;
    QPushButton* m_stop_btn;
    QPushButton* m_show_info_btn;

    // Evolution chart
    QtCharts::QLineSeries* m_world_size_series;
    QtCharts::QLineSeries* m_species_count_series;
    QtCharts::QLineSeries* m_new_entities_series;
    QtCharts::QLineSeries* m_entities_died_series;
    QtCharts::QLineSeries* m_species_died_series;

    // One generation chart
    // TODO: use bar chart instead
    QtCharts::QLineSeries* m_species_series;
    QtCharts::QChartView* m_species_chart_view;

    // Max fitness chart
    QtCharts::QLineSeries* m_max_fitness_series;
    double m_max_fitness;

    // Fitness landscape chart
    QtCharts::QLineSeries* m_fitness_series;
    QtCharts::QChartView* m_fitness_chart_view;

    // Entities fitness-norm chart
    QtCharts::QChartView* m_norm_chart_view;

    SettingsWidget m_settings_widget;

    std::thread m_thread;
    std::unique_ptr<EvolutionWorker> m_worker;
};


#endif //GUI_EVOLUTIONWIDGET_H

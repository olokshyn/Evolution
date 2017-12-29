//
// Created by oleg on 23.12.17.
//

#ifndef EVOLUTION_GRAPHWIDGET_H
#define EVOLUTION_GRAPHWIDGET_H

#include <thread>
#include <atomic>

#include <QWidget>
#include <QPointF>
#include <QList>

extern "C"
{
#include "GeneticAlgorithm/GAObjective.h"
};

class QLabel;

class CustomChartView;

class GraphWidget : public QWidget
{
Q_OBJECT

public:
    struct GraphParameters
    {
        explicit GraphParameters(const Objective* objective = nullptr,
                                 size_t args_count = 0,
                                 size_t detalization_level = 100,
                                 size_t update_progress_times = 10000);

        const Objective* objective;
        size_t args_count;
        size_t detalization_level;
        size_t update_progress_times;
    };

public:
    explicit GraphWidget(QWidget* parent = nullptr);
    ~GraphWidget() override;

    void plot(const GraphParameters& parameters);

signals:
    void plotting_progress(double percents);
    void twod_data_ready(const QList<QPointF>& data,
                         double min_x, double max_x,
                         double min_y, double max_y);
    void norms_data_ready(const QList<QPointF>& min_norms,
                          const QList<QPointF>& max_norms,
                          double min_norm, double max_norm,
                          double global_objective_min,
                          double global_objective_max);

public slots:
    void clear_graph();
    void stop_plotting();
    void plot_twod_data(const QList<QPointF>& data,
                        double min_x, double max_x,
                        double min_y, double max_y);
    void plot_norms_data(const QList<QPointF>& min_norms,
                         const QList<QPointF>& max_norms,
                         double min_norm, double max_norm,
                         double global_objective_min,
                         double global_objective_max);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void update_loading(double percents);

private:
    void plot_twod(const GraphParameters& parameters);
    void plot_norms(const GraphParameters& parameters);

private:
    std::thread m_plotter_thread;
    std::atomic<bool> m_stop_plotting;
    CustomChartView* m_chart_view;
    QLabel* m_loading_lbl;
};


#endif //EVOLUTION_GRAPHWIDGET_H

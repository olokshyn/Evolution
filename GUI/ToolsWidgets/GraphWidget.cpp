//
// Created by oleg on 23.12.17.
//

#include "GraphWidget.h"

#include <vector>
#include <map>
#include <functional>
#include <numeric>
#include <limits>

#include <QLabel>
#include <QLineSeries>
#include <QBoxLayout>

extern "C"
{
#include "Common.h"
#include "Logging/Logging.h"
}

using namespace QtCharts;

namespace
{

const double offset = 0.3;
const size_t g_graph_steps_count = 100;

bool __for_each_point_in_space(std::vector<double>& point, size_t index,
                               double a, double b, double step,
                               std::function<bool(const std::vector<double>&)>& callback)
{
    if (index >= point.size())
    {
        return false;
    }
    for (double x = a; x <= b; x += step)
    {
        point[index] = x;
        if (index + 1 == point.size())
        {
            if (!callback(point))
            {
                return false;
            }
        }
        else
        {
            if (!__for_each_point_in_space(point, index + 1, a, b, step, callback))
            {
                return false;
            }
        }
    }
    return true;
}

void for_each_point_in_space(size_t args_count,
                             double a, double b, double step,
                             std::function<bool(const std::vector<double>&)> callback)
{
    std::vector<double> point(args_count);
    size_t index = 0;
    __for_each_point_in_space(point, index, a, b, step, callback);
}

bool double_less(double left, double right)
{
    return DOUBLE_LT(left, right);
}

}

GraphWidget::GraphWidget(QWidget* parent)
        : QWidget(parent),
          m_stop_plotting(false),
          m_chart_view(new QChartView(this)),
          m_loading_lbl(new QLabel(this))
{
    m_chart_view->setChart(new QChart());

    connect(this, &GraphWidget::norms_data_ready,
            this, &GraphWidget::plot_norms_data,
            Qt::QueuedConnection);
    connect(this, &GraphWidget::plotting_progress,
            this, &GraphWidget::update_loading,
            Qt::QueuedConnection);

    auto layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->addWidget(m_chart_view);
    m_loading_lbl->resize(100, 50);
    m_loading_lbl->setAlignment(Qt::AlignCenter);
    m_loading_lbl->setStyleSheet("QLabel { color: rgb(100, 100, 100); }");
    m_loading_lbl->setText("0.00%");
}

GraphWidget::~GraphWidget()
{
    stop_plotting();
}

void GraphWidget::plot(const Objective* objective, size_t args_count)
{
    if (args_count == 0)
    {
        return;
    }
    clear_graph();
    m_stop_plotting = false;
    m_plotter_thread = std::thread([this, objective, args_count]() {
        plot_in_norms(objective, args_count);
    });
}

void GraphWidget::clear_graph()
{
    stop_plotting();
    m_chart_view->chart()->removeAllSeries();
    auto axes = m_chart_view->chart()->axes();
    for (auto axis : axes)
    {
        m_chart_view->chart()->removeAxis(axis);
        delete axis;
    }
    m_chart_view->repaint();
    m_loading_lbl->setText("0.00%");
    m_loading_lbl->setVisible(true);
}

void GraphWidget::stop_plotting()
{
    m_stop_plotting = true;
    if (m_plotter_thread.joinable())
    {
        m_plotter_thread.join();
    }
}

void GraphWidget::plot_norms_data(const QList<QPointF>& min_norms,
                                  const QList<QPointF>& max_norms,
                                  double min_norm, double max_norm,
                                  double global_objective_min,
                                  double global_objective_max)
{
    m_loading_lbl->setVisible(false);

    QLineSeries* min_series = new QLineSeries(this);
    min_series->setName("Min");
    min_series->setColor(QColor(255, 0, 30));
    QLineSeries* max_series = new QLineSeries(this);
    max_series->setName("Max");
    max_series->setColor(QColor(0, 255, 30));

    min_series->append(min_norms);
    max_series->append(max_norms);

    auto chart = m_chart_view->chart();
    chart->addSeries(min_series);
    chart->addSeries(max_series);
    chart->createDefaultAxes();
    chart->axisX()->setRange(min_norm - offset * fabs(min_norm),
                             max_norm + offset * fabs(max_norm));
    chart->axisY()->setRange(global_objective_min - 0.5 * offset * fabs(global_objective_min),
                             global_objective_max + 2 * offset * fabs(global_objective_max));
}

void GraphWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_loading_lbl->move(rect().center() - m_loading_lbl->rect().center());
}

void GraphWidget::closeEvent(QCloseEvent* event)
{
    clear_graph();
    QWidget::closeEvent(event);
}

void GraphWidget::update_loading(double percents)
{
    m_loading_lbl->setText(QString::number(percents, 'f', 2) + "%");
}

void GraphWidget::plot_in_norms(const Objective* objective, size_t args_count)
{
    // Don`t use DOUBLE_GT, we need a positive value further
    LOG_RELEASE_ASSERT(objective->max > objective->min);
    double step = (objective->max - objective->min) / g_graph_steps_count;

    unsigned long long iterations_count =
            static_cast<unsigned long long>(powl(g_graph_steps_count + 1, args_count));
    unsigned long long iterations_done = 0;
    unsigned long long update_status_iterations = iterations_count / 10000;

    // NOTE: double_less does not provide a total order, this may cause issues.
    // norm: [objective min value, objective max value]
    std::map<double, std::pair<double, double>, decltype(&double_less)> norms(&double_less);
    for_each_point_in_space(
            args_count, objective->min, objective->max, step,
            [this, objective, &norms, &iterations_done, &iterations_count, &update_status_iterations]
                    (const std::vector<double>& point) -> bool {
                LOG_ASSERT(!point.empty());
                double norm = sqrt(std::accumulate(point.begin(), point.end(), 0.0,
                                                   [](double left, double right) -> double {
                                                       return left + right * right;
                                                   }));
                double objective_value = objective->func(point.data(), point.size());
                auto iter = norms.find(norm);
                if (iter == norms.end())
                {
                    norms.insert(std::make_pair(norm, std::make_pair(objective_value, objective_value)));
                }
                else
                {
                    iter->second.first = MIN(iter->second.first, objective_value);
                    iter->second.second = MAX(iter->second.second, objective_value);
                }
                ++iterations_done;
                if (iterations_done % update_status_iterations == 0)
                {
                    emit plotting_progress(
                            static_cast<double>(
                                    static_cast<long double>(iterations_done)
                                    / static_cast<long double>(iterations_count))
                            * 100.0);
                }
                return !m_stop_plotting;
            });
    if (m_stop_plotting)
    {
        return;
    }
    // There must be at least one point in the range
    LOG_RELEASE_ASSERT(!norms.empty());

    double max_norm = sqrt(args_count * pow(MAX(fabs(objective->min), fabs(objective->max)), 2.0));
    double global_objective_min = std::numeric_limits<double>::max();
    double global_objective_max = -std::numeric_limits<double>::max();

    QList<QPointF> min_norms;
    QList<QPointF> max_norms;
    for (const auto& item : norms)
    {
        LOG_ASSERT(DOUBLE_GE(max_norm, item.first));
        min_norms.append(QPointF(item.first, item.second.first));
        max_norms.append(QPointF(item.first, item.second.second));
        global_objective_min = MIN(global_objective_min, item.second.first);
        global_objective_max = MAX(global_objective_max, item.second.second);
    }
    emit norms_data_ready(min_norms, max_norms,
                          0.0, max_norm,
                          global_objective_min, global_objective_max);
}

//
// Created by Oleg on 11/19/17.
//

#include "EvolutionWidget.h"

#include <algorithm>
#include <limits>
#include <iostream>

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QBoxLayout>
#include <QChart>
#include <QValueAxis>
#include <QTabWidget>

#include "EvolutionWorker.h"
#include "Utils/Utils.h"

using namespace QtCharts;

namespace
{
    const double offset = 5.0;

    QChartView* create_evolution_chart(
            const QString& chartTitle,
            QWidget* parent)
    {
        auto chart = new QChart();
        chart->setTitle(chartTitle);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        auto chartView = new QChartView(chart, parent);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumHeight(300);
        return chartView;
    }

    QChartView* create_species_chart(
            QAbstractSeries* series,
            const QString& chartTitle,
            QWidget* parent)
    {
        auto chart = new QChart();
        chart->setTitle(chartTitle);
        chart->addSeries(series);
        chart->legend()->hide();
        chart->createDefaultAxes();
        auto chartView = new QChartView(chart, parent);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumHeight(200);
        return chartView;
    }

    QChartView* create_max_fitness_chart(
            QAbstractSeries* series,
            size_t max_generations_count,
            const QString& chartTitle,
            double min_fitness,
            double max_fitness,
            QWidget* parent)
    {
        auto chart = new QChart();
        chart->setTitle(chartTitle);
        chart->addSeries(series);
        chart->legend()->hide();
        chart->createDefaultAxes();
        chart->axisX()->setRange(
                0, static_cast<qulonglong>(max_generations_count));
        chart->axisY()->setRange(min_fitness, max_fitness);
        auto chartView = new QChartView(chart, parent);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumHeight(200);
        return chartView;
    }

    QChartView* create_fitness_chart(
            QAbstractSeries* series,
            const QString& chartTitle,
            QWidget* parent)
    {
        auto chart = new QChart();
        chart->setTitle(chartTitle);
        chart->addSeries(series);
        chart->legend()->hide();
        chart->createDefaultAxes();
        auto chartView = new QChartView(chart, parent);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumHeight(200);
        return chartView;
    }

    QChartView* create_norms_chart(
            QAbstractSeries* series,
            const QString& chartTitle,
            QWidget* parent)
    {
        auto chart = new QChart();
        chart->setTitle(chartTitle);
        chart->addSeries(series);
        chart->legend()->hide();
        chart->createDefaultAxes();
        auto chartView = new QChartView(chart, parent);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumHeight(200);
        return chartView;
    }
}

EvolutionWidget::EvolutionWidget(const SettingsWidget& settings,
                                 const std::string& name,
                                 QWidget* parent)
        : QWidget(parent),
          m_generation_number_lbl(new QLabel(this)),
          m_max_fitness_lbl(new QLabel(this)),
          m_stop_btn(new QPushButton("Stop evolution", this)),
          m_show_info_btn(new QPushButton("Show parameters", this)),

          m_world_size_series(new QLineSeries(this)),
          m_species_count_series(new QLineSeries(this)),
          m_new_entities_series(new QLineSeries(this)),
          m_entities_died_series(new QLineSeries(this)),
          m_species_died_series(new QLineSeries(this)),

          m_species_series(new QLineSeries(this)),
          m_species_chart_view(nullptr),

          m_max_fitness_series(new QLineSeries(this)),
        // TODO: is it safe to do so?
          m_max_fitness(-std::numeric_limits<double>::max()),

          m_fitness_series(new QLineSeries(this)),
          m_fitness_chart_view(nullptr),

          m_norms_series(new QLineSeries(this)),
          m_norm_chart_view(nullptr),

          m_settings_widget(settings.parameters(),
                            settings.operators()),

          m_worker(new EvolutionWorker(settings.parameters(),
                                       settings.operators(),
                                       settings.ui_settings().iterations_buffer_size,
                                       name))
{
    m_world_size_series->setName("World size");
    m_species_count_series->setName("Species count");
    m_new_entities_series->setName(("New entities count"));
    m_entities_died_series->setName("Entities died");
    m_species_died_series->setName("Species died");
    m_species_series->setName("Species");
    m_max_fitness_series->setName("Max fitness");
    m_fitness_series->setName("Fitness landscape");
    m_norms_series->setName("Fitness-Norm");

    connect(m_stop_btn, &QPushButton::clicked,
            this, &EvolutionWidget::stop_evolution);
    connect(m_show_info_btn, &QPushButton::clicked,
            this, &EvolutionWidget::show_info);

    connect(m_worker.get(), &EvolutionWorker::iterations_done,
            this, &EvolutionWidget::plot_iterations,
            Qt::QueuedConnection);
    connect(m_worker.get(), &EvolutionWorker::optimum_reached,
            this, &EvolutionWidget::optimum_reached,
            Qt::QueuedConnection);

    auto evolution_chart = create_evolution_chart("Evolution", this);
    add_evolution_serieses(evolution_chart,
                           settings.parameters().max_generations_count,
                           settings.parameters().initial_world_size);

    auto row_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    row_layout->addWidget(new QLabel("Generation number:", this));
    row_layout->addWidget(m_generation_number_lbl, 1);

    row_layout->addWidget(new QLabel("Max fitness:", this));
    row_layout->addWidget(m_max_fitness_lbl, 1);

    row_layout->addWidget(m_stop_btn, 1);
    row_layout->addWidget(m_show_info_btn, 1);

    auto layout = new QGridLayout();
    layout->addLayout(
            row_layout,
            0, 0, 1, 8);
    layout->addWidget(
            evolution_chart,
            1, 0, 2, 8);
    m_species_chart_view = create_species_chart(
            m_species_series,
            "Species",
            this);
    layout->addWidget(
            m_species_chart_view,
            3, 0, 1, 4);
    layout->addWidget(
            create_max_fitness_chart(m_max_fitness_series,
                                     settings.parameters().max_generations_count,
                                     "Max fitness",
                                     settings.ui_settings().min_fitness,
                                     settings.ui_settings().max_fitness,
                                     this),
            3, 4, 1, 4);
    m_fitness_chart_view = create_fitness_chart(
            m_fitness_series,
            "Fitness landscape", this);
    layout->addWidget(
            m_fitness_chart_view,
            4, 0, 1, 4);
    m_norm_chart_view = create_norms_chart(
            m_norms_series,
            "Fitness-Norm",
            this);
    layout->addWidget(
            m_norm_chart_view,
            4, 4, 1, 4);
    this->setLayout(layout);
}

EvolutionWidget::~EvolutionWidget()
{
    if (m_thread.joinable())
    {
        m_worker->stop_evolution();
        m_thread.join();
    }
}

void EvolutionWidget::start_evolution()
{
    m_thread = std::thread([this]() {
        try
        {
            m_worker->start_evolution();
        }
        catch (const std::exception& exc)
        {
            std::cerr << "Exception: " << exc.what() << std::endl;
        }
    });
}

void EvolutionWidget::stop_evolution()
{
    m_worker->stop_evolution();
}

void EvolutionWidget::show_info()
{
    m_settings_widget.show();
}

void EvolutionWidget::plot_iterations(const QList<IterationInfo>& infos)
{
    if (infos.empty())
    {
        return;
    }

    struct {
        QList<QPointF> world_size;
        QList<QPointF> species_count;
        QList<QPointF> new_entities;
        QList<QPointF> entities_died;
        QList<QPointF> species_died;
        QList<QPointF> max_fitness;
    } infos_data{};
    for (const auto& info : infos)
    {
        infos_data.world_size.append(
                QPointF(info.generation_number, info.world_size));
        infos_data.species_count.append(
                QPointF(info.generation_number, info.species.size()));
        infos_data.new_entities.append(
                QPointF(info.generation_number, info.new_entities_count));
        infos_data.entities_died.append(
                QPointF(info.generation_number, info.entities_died));
        infos_data.species_died.append(
                QPointF(info.generation_number, info.species_died));

        if (!info.fitnesses.empty())
        {
            double max_fitness = *std::max_element(info.fitnesses.cbegin(),
                                                   info.fitnesses.cend());
            if (max_fitness > m_max_fitness)
            {
                m_max_fitness = max_fitness;
            }
            infos_data.max_fitness.append(
                    QPointF(info.generation_number, max_fitness));
        }
    }
    m_world_size_series->append(infos_data.world_size);
    m_species_count_series->append(infos_data.species_count);
    m_new_entities_series->append(infos_data.new_entities);
    m_entities_died_series->append(infos_data.entities_died);
    m_species_died_series->append(infos_data.species_died);
    m_max_fitness_series->append(infos_data.max_fitness);

    const auto& last_info = infos.last();

    m_generation_number_lbl->setText(QString::number(last_info.generation_number));
    m_max_fitness_lbl->setText(QString::number(m_max_fitness));

    QList<QPointF> points;
    size_t max_species_size = 0;
    for (size_t i = 0; i != last_info.species.size(); ++i)
    {
        points.append(QPointF(i, last_info.species[i]));
        if (last_info.species[i] > max_species_size)
        {
            max_species_size = last_info.species[i];
        }
    }
    m_species_series->clear();
    if (!last_info.species.empty())
    {
        m_species_chart_view->chart()->axisX()->setRange(
                0,
                static_cast<qulonglong>(last_info.species.size() + 1));
        m_species_chart_view->chart()->axisY()->setRange(
                0,
                static_cast<qulonglong>(max_species_size + 1));
    }
    m_species_series->append(points);

    points.clear();
    double min_fitness = std::numeric_limits<double>::max();
    for (size_t i = 0; i != last_info.fitnesses.size(); ++i)
    {
        points.append(QPointF(i, last_info.fitnesses[i]));
        if (last_info.fitnesses[i] < min_fitness)
        {
            min_fitness = last_info.fitnesses[i];
        }
    }
    m_fitness_series->clear();
    if (!last_info.fitnesses.empty())
    {
        m_fitness_chart_view->chart()->axisX()->setRange(
                0,
                static_cast<qulonglong>(last_info.fitnesses.size() + 1));
        m_fitness_chart_view->chart()->axisY()->setRange(min_fitness - offset,
                                                         m_max_fitness + offset);
    }
    m_fitness_series->append(points);

    points.clear();
    LOG_ASSERT(last_info.fitnesses.size() == last_info.norms.size());
    for (size_t i = 0; i != last_info.norms.size(); ++i)
    {
        points.append(QPointF(last_info.norms[i],
                              last_info.fitnesses[i]));
        LOG_ASSERT(DOUBLE_LE(last_info.fitnesses[i], m_max_fitness));
        LOG_ASSERT(DOUBLE_GE(last_info.fitnesses[i], min_fitness));
    }
    m_norms_series->clear();
    if (!last_info.norms.empty())
    {
        double max_norm = *std::max_element(last_info.norms.cbegin(),
                                            last_info.norms.cend());
        m_norm_chart_view->chart()->axisX()->setRange(0, max_norm + 1);
        m_norm_chart_view->chart()->axisY()->setRange(min_fitness - offset,
                                                      m_max_fitness + offset);
    }
    m_norms_series->append(points);
}

void EvolutionWidget::optimum_reached(double optimum)
{
    m_max_fitness_lbl->setText(QString::number(optimum) + " (optimum)");
}

void EvolutionWidget::add_evolution_serieses(QtCharts::QChartView* chart_view,
                                             size_t max_generations_count,
                                             size_t world_size)
{
    auto chart = chart_view->chart();

    chart->addSeries(m_world_size_series);
    chart->addSeries(m_species_count_series);
    chart->addSeries(m_new_entities_series);
    chart->addSeries(m_entities_died_series);
    chart->addSeries(m_species_died_series);

    auto axisX = new QValueAxis();
    axisX->setRange(0, static_cast<qulonglong>(max_generations_count));
    chart->setAxisX(axisX, m_world_size_series);
    chart->setAxisX(axisX, m_species_count_series);
    chart->setAxisX(axisX, m_new_entities_series);
    chart->setAxisX(axisX, m_entities_died_series);
    chart->setAxisX(axisX, m_species_died_series);

    auto axisY = new QValueAxis();
    axisY->setRange(0, static_cast<qulonglong>(1.2 * world_size));
    chart->setAxisY(axisY, m_world_size_series);
    chart->setAxisY(axisY, m_species_count_series);
    chart->setAxisY(axisY, m_new_entities_series);
    chart->setAxisY(axisY, m_entities_died_series);
    chart->setAxisY(axisY, m_species_died_series);
}
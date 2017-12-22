//
// Created by Oleg on 11/25/17.
//

#include "SettingsWidget.h"

#include <utility>

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QStringList>
#include <QSettings>

extern "C"
{
#include "PluginManager/PluginManager.h"
}

SettingsWidget::SettingsWidget(QWidget* parent)
        : QWidget(parent),
          m_read_only(false),

          m_current_objective(0),
          m_current_operators(0),

        // parameters
          m_initial_world_size_edit(new QLineEdit(this)),
          m_chromosome_size_edit(new QLineEdit(this)),
          m_mutation_probability_edit(new QLineEdit(this)),
          m_mutation_on_iteration_dependence_edit(new QLineEdit(this)),
          m_crossover_probability_edit(new QLineEdit(this)),
          m_selection_worst_probability_edit(new QLineEdit(this)),
          m_selection_best_probability_edit(new QLineEdit(this)),
          m_selection_elitists_count_edit(new QLineEdit(this)),
          m_min_pts_edit(new QLineEdit(this)),
          m_eps_edit(new QLineEdit(this)),
          m_objective_selector(new QComboBox(this)),
          m_max_generations_count_edit(new QLineEdit(this)),
          m_stable_value_iterations_count_edit(new QLineEdit(this)),
          m_stable_value_eps_edit(new QLineEdit(this)),
          m_species_link_iterations_count_edit(new QLineEdit(this)),
          m_species_link_probability_edit(new QLineEdit(this)),
          m_species_link_min_edit(new QLineEdit(this)),
          m_species_link_max_edit(new QLineEdit(this)),

        // operators
          m_operators_selector(new QComboBox(this)),

        // ui settings
          m_min_fitness_edit(new QLineEdit(this)),
          m_max_fitness_edit(new QLineEdit(this)),
          m_max_species_count_edit(new QLineEdit(this)),
          m_iterations_buffer_size_edit(new QLineEdit(this))
{
    QSettings settings;

    m_current_objective = settings.value("current_objective", 0).toULongLong();
    check_current_objective();
    m_current_operators = settings.value("current_operators", 0).toULongLong();
    check_current_operators();

    m_parameters = {
            .initial_world_size =
            settings.value("initial_world_size", 61).toULongLong(),

            .chromosome_size =
            settings.value("chromosome_size", 25).toULongLong(),

            .mutation_probability =
            settings.value("mutation_probability", 0.2).toDouble(),

            .mutation_on_iteration_dependence =
            settings.value("mutation_on_iteration_dependence", 5.0).toDouble(),

            .crossover_probability =
            settings.value("crossover_probability", 0.6).toDouble(),

            .selection_worst_probability =
            settings.value("selection_worst_probability", 0.5).toDouble(),

            .selection_best_probability =
            settings.value("selection_best_probability", 1.5).toDouble(),

            .selection_elitists_count =
            settings.value("selection_elitists_count", 5).toULongLong(),

            .min_pts =
            settings.value("min_pts", 1).toULongLong(),

            .eps =
            settings.value("eps", 0.09).toDouble(),

            .objective = {},

            .max_generations_count =
            settings.value("max_generations_count", 500).toULongLong(),

            .stable_value_iterations_count =
            settings.value("stable_value_iterations_count", 1000).toULongLong(),

            .stable_value_eps =
            settings.value("stable_value_eps", 1e-5).toDouble(),

            .species_link_iterations_count =
            settings.value("species_link_iterations_count", 5).toULongLong(),

            .species_link_probability =
            settings.value("species_link_probability", 0.5).toDouble(),

            .species_link_min =
            settings.value("species_link_min", -1.0).toDouble(),

            .species_link_max =
            settings.value("species_link_max", 1.0).toDouble()
    };

    m_parameters.objective = *get_current_objective();
    m_operators = *get_current_operators();

    m_ui_settings = {
            .min_fitness =
            settings.value("min_fitness", -300.0).toDouble(),

            .max_fitness =
            settings.value("max_fitness", 0.0).toDouble(),

            .max_species_count =
            settings.value("max_species_count", 50).toULongLong(),

            .iterations_buffer_size =
            settings.value("iterations_buffer_size", 1).toULongLong()
    };

    init_widget();
}

SettingsWidget::SettingsWidget(const GAParameters& parameters,
                               const GAOperators& operators,
                               QWidget* parent)
        : QWidget(parent),
          m_read_only(true),

          m_parameters(parameters),
          m_operators(operators),

          m_current_objective(0),
          m_current_operators(0),

        // parameters
          m_initial_world_size_edit(new QLineEdit(this)),
          m_chromosome_size_edit(new QLineEdit(this)),
          m_mutation_probability_edit(new QLineEdit(this)),
          m_mutation_on_iteration_dependence_edit(new QLineEdit(this)),
          m_crossover_probability_edit(new QLineEdit(this)),
          m_selection_worst_probability_edit(new QLineEdit(this)),
          m_selection_best_probability_edit(new QLineEdit(this)),
          m_selection_elitists_count_edit(new QLineEdit(this)),
          m_min_pts_edit(new QLineEdit(this)),
          m_eps_edit(new QLineEdit(this)),
          m_objective_selector(new QComboBox(this)),
          m_max_generations_count_edit(new QLineEdit(this)),
          m_stable_value_iterations_count_edit(new QLineEdit(this)),
          m_stable_value_eps_edit(new QLineEdit(this)),
          m_species_link_iterations_count_edit(new QLineEdit(this)),
          m_species_link_probability_edit(new QLineEdit(this)),
          m_species_link_min_edit(new QLineEdit(this)),
          m_species_link_max_edit(new QLineEdit(this)),

        // operators
          m_operators_selector(new QComboBox(this)),

        // ui settings
          m_min_fitness_edit(nullptr),
          m_max_fitness_edit(nullptr),
          m_max_species_count_edit(nullptr),
          m_iterations_buffer_size_edit(nullptr)
{
    find_current_objective();
    find_current_operators();

    m_initial_world_size_edit->setReadOnly(true);
    m_chromosome_size_edit->setReadOnly(true);
    m_mutation_probability_edit->setReadOnly(true);
    m_mutation_on_iteration_dependence_edit->setReadOnly(true);
    m_crossover_probability_edit->setReadOnly(true);
    m_selection_worst_probability_edit->setReadOnly(true);
    m_selection_best_probability_edit->setReadOnly(true);
    m_selection_elitists_count_edit->setReadOnly(true);
    m_min_pts_edit->setReadOnly(true);
    m_eps_edit->setReadOnly(true);
    m_objective_selector->setEnabled(false);
    m_max_generations_count_edit->setReadOnly(true);
    m_stable_value_iterations_count_edit->setReadOnly(true);
    m_stable_value_eps_edit->setReadOnly(true);
    m_species_link_iterations_count_edit->setReadOnly(true);
    m_species_link_probability_edit->setReadOnly(true);
    m_species_link_min_edit->setReadOnly(true);
    m_species_link_max_edit->setReadOnly(true);

    m_operators_selector->setEnabled(false);

    init_widget();
}

const GAParameters& SettingsWidget::parameters() const
{
    return m_parameters;
}

const GAOperators& SettingsWidget::operators() const
{
    return m_operators;
}

const SettingsWidget::UISettings& SettingsWidget::ui_settings() const
{
    return m_ui_settings;
}

void SettingsWidget::save_settings()
{
    save_parameters();
    save_operators();
    if (!m_read_only)
    {
        save_ui_settings();
    }

    emit settings_changed();
}

void SettingsWidget::reset_settings()
{
    reset_parameters();
    reset_operators();
    if (!m_read_only)
    {
        reset_ui_settings();
    }
}

void SettingsWidget::init_widget()
{
    auto layout = new QGridLayout(this);

    layout->addWidget(create_evolution_box(), 0, 0, 2, 1);
    layout->addWidget(create_operators_box(), 0, 1);
    if (!m_read_only)
    {
        layout->addWidget(create_ui_settings_box(), 1, 1);
    }

    reset_settings();
}

QGroupBox* SettingsWidget::create_evolution_box()
{
    auto group_box = new QGroupBox(this);
    group_box->setTitle("Evolution parameters");

    auto layout = new QGridLayout(group_box);

    layout->addWidget(new QLabel("Initial world size", this), 0, 0);
    layout->addWidget(m_initial_world_size_edit, 0, 1);

    layout->addWidget(new QLabel("Chromosome size", this), 1, 0);
    layout->addWidget(m_chromosome_size_edit, 1, 1);

    layout->addWidget(new QLabel("Mutation probability", this), 2, 0);
    layout->addWidget(m_mutation_probability_edit, 2, 1);

    layout->addWidget(new QLabel("Mutation on iteration dependence", this), 3, 0);
    layout->addWidget(m_mutation_on_iteration_dependence_edit, 3, 1);

    layout->addWidget(new QLabel("Crossover probability", this), 4, 0);
    layout->addWidget(m_crossover_probability_edit, 4, 1);

    layout->addWidget(new QLabel("Selection worst probability", this), 5, 0);
    layout->addWidget(m_selection_worst_probability_edit, 5, 1);

    layout->addWidget(new QLabel("Selection best probability", this), 6, 0);
    layout->addWidget(m_selection_best_probability_edit);

    layout->addWidget(new QLabel("Selection elitists count", this), 7, 0);
    layout->addWidget(m_selection_elitists_count_edit, 7, 1);

    layout->addWidget(new QLabel("Min pts", this), 8, 0);
    layout->addWidget(m_min_pts_edit, 8, 1);

    layout->addWidget(new QLabel("Eps", this), 9, 0);
    layout->addWidget(m_eps_edit, 9, 1);

    layout->addWidget(new QLabel("Objective function", this), 10, 0);
    layout->addWidget(m_objective_selector, 10, 1);

    layout->addWidget(new QLabel("Max generations count", this), 11, 0);
    layout->addWidget(m_max_generations_count_edit, 11, 1);

    layout->addWidget(new QLabel("Stable value iterations count", this), 12, 0);
    layout->addWidget(m_stable_value_iterations_count_edit, 12, 1);

    layout->addWidget(new QLabel("Stable value eps", this), 13, 0);
    layout->addWidget(m_stable_value_eps_edit, 13, 1);

    layout->addWidget(new QLabel("Species link iterations count", this), 14, 0);
    layout->addWidget(m_species_link_iterations_count_edit, 14, 1);

    layout->addWidget(new QLabel("Species link probability", this), 15, 0);
    layout->addWidget(m_species_link_probability_edit, 15, 1);

    layout->addWidget(new QLabel("Species link min", this), 16, 0);
    layout->addWidget(m_species_link_min_edit, 16, 1);

    layout->addWidget(new QLabel("Species link max", this), 17, 0);
    layout->addWidget(m_species_link_max_edit, 17, 1);

    return group_box;
}

QGroupBox* SettingsWidget::create_operators_box()
{
    auto group_box = new QGroupBox(this);
    group_box->setTitle("Evolution operators");

    auto layout = new QGridLayout(group_box);

    layout->addWidget(new QLabel("Operators", this), 0, 0);
    layout->addWidget(m_operators_selector, 0, 1);

    return group_box;
}

QGroupBox* SettingsWidget::create_ui_settings_box()
{
    auto group_box = new QGroupBox(this);
    group_box->setTitle("UI settings");

    auto layout = new QGridLayout(group_box);

    layout->addWidget(new QLabel("Min fitness", this), 0, 0);
    layout->addWidget(m_min_fitness_edit, 0, 1);

    layout->addWidget(new QLabel("Max fitness", this), 1, 0);
    layout->addWidget(m_max_fitness_edit, 1, 1);

    layout->addWidget(new QLabel("Max species count", this), 2, 0);
    layout->addWidget(m_max_species_count_edit, 2, 1);

    layout->addWidget(new QLabel("Iterations buffer size", this), 3, 0);
    layout->addWidget(m_iterations_buffer_size_edit, 3, 1);

    return group_box;
}

void SettingsWidget::save_parameters()
{
    m_current_objective =
            static_cast<size_t>(m_objective_selector->currentIndex());
    check_current_objective();

    m_parameters.initial_world_size =
            m_initial_world_size_edit->text().toULongLong();
    m_parameters.chromosome_size =
            m_chromosome_size_edit->text().toULongLong();
    m_parameters.mutation_probability =
            m_mutation_probability_edit->text().toDouble();
    m_parameters.mutation_on_iteration_dependence =
            m_mutation_on_iteration_dependence_edit->text().toDouble();
    m_parameters.crossover_probability =
            m_crossover_probability_edit->text().toDouble();
    m_parameters.selection_worst_probability =
            m_selection_worst_probability_edit->text().toDouble();
    m_parameters.selection_best_probability =
            m_selection_best_probability_edit->text().toDouble();
    m_parameters.selection_elitists_count =
            m_selection_elitists_count_edit->text().toULongLong();
    m_parameters.min_pts =
            m_min_pts_edit->text().toULongLong();
    m_parameters.eps =
            m_eps_edit->text().toDouble();
    m_parameters.objective = *get_current_objective();
    m_parameters.max_generations_count =
            m_max_generations_count_edit->text().toULongLong();
    m_parameters.stable_value_iterations_count =
            m_stable_value_iterations_count_edit->text().toULongLong();
    m_parameters.stable_value_eps =
            m_stable_value_eps_edit->text().toDouble();
    m_parameters.species_link_iterations_count =
            m_species_link_iterations_count_edit->text().toULongLong();
    m_parameters.species_link_probability =
            m_species_link_probability_edit->text().toDouble();
    m_parameters.species_link_min =
            m_species_link_min_edit->text().toDouble();
    m_parameters.species_link_max =
            m_species_link_max_edit->text().toDouble();

    QSettings settings;

    settings.setValue(
            "current_objective",
            static_cast<qulonglong>(m_current_objective));

    settings.setValue(
            "initial_world_size",
            static_cast<qulonglong>(m_parameters.initial_world_size));
    settings.setValue(
            "chromosome_size",
            static_cast<qulonglong>(m_parameters.chromosome_size));
    settings.setValue(
            "mutation_probability",
            m_parameters.mutation_probability);
    settings.setValue(
            "mutation_on_iteration_dependence",
            m_parameters.mutation_on_iteration_dependence);
    settings.setValue(
            "crossover_probability",
            m_parameters.crossover_probability);
    settings.setValue(
            "selection_worst_probability",
            m_parameters.selection_worst_probability);
    settings.setValue(
            "selection_best_probability",
            m_parameters.selection_best_probability);
    settings.setValue(
            "selection_elitists_count",
            static_cast<qulonglong>(m_parameters.selection_elitists_count));
    settings.setValue(
            "min_pts",
            static_cast<qulonglong>(m_parameters.min_pts));
    settings.setValue(
            "eps",
            m_parameters.eps);
    // objective
    settings.setValue(
            "max_generations_count",
            static_cast<qulonglong>(m_parameters.max_generations_count));
    settings.setValue(
            "stable_value_iterations_count",
            static_cast<qulonglong>(m_parameters.stable_value_iterations_count));
    settings.setValue(
            "stable_value_eps",
            m_parameters.stable_value_eps);
    settings.setValue(
            "species_link_iterations_count",
            static_cast<qulonglong>(m_parameters.species_link_iterations_count));
    settings.setValue(
            "species_link_probability",
            m_parameters.species_link_probability);
    settings.setValue(
            "species_link_min",
            m_parameters.species_link_min);
    settings.setValue(
            "species_link_max",
            m_parameters.species_link_max);

    settings.sync();
}

void SettingsWidget::reset_parameters()
{
    check_current_objective();

    m_initial_world_size_edit->setText(
            QString::number(m_parameters.initial_world_size));
    m_chromosome_size_edit->setText(
            QString::number(m_parameters.chromosome_size));
    m_mutation_probability_edit->setText(
            QString::number(m_parameters.mutation_probability));
    m_mutation_on_iteration_dependence_edit->setText(
            QString::number(m_parameters.mutation_on_iteration_dependence));
    m_crossover_probability_edit->setText(
            QString::number(m_parameters.crossover_probability));
    m_selection_worst_probability_edit->setText(
            QString::number(m_parameters.selection_worst_probability));
    m_selection_best_probability_edit->setText(
            QString::number(m_parameters.selection_best_probability));
    m_selection_elitists_count_edit->setText(
            QString::number(m_parameters.selection_elitists_count));
    m_min_pts_edit->setText(
            QString::number(m_parameters.min_pts));
    m_eps_edit->setText(
            QString::number(m_parameters.eps));

    QStringList items;
    list_for_each(ConstObjectivePtr, g_plugin_objectives, var)
    {
        items.append(list_var_value(var)->name);
    }
    m_objective_selector->clear();
    m_objective_selector->addItems(items);
    m_objective_selector->setCurrentIndex(static_cast<int>(m_current_objective));

    m_max_generations_count_edit->setText(
            QString::number(m_parameters.max_generations_count));
    m_stable_value_iterations_count_edit->setText(
            QString::number(m_parameters.stable_value_iterations_count));
    m_stable_value_eps_edit->setText(
            QString::number(m_parameters.stable_value_eps));
    m_species_link_iterations_count_edit->setText(
            QString::number(m_parameters.species_link_iterations_count));
    m_species_link_probability_edit->setText(
            QString::number(m_parameters.species_link_probability));
    m_species_link_min_edit->setText(
            QString::number(m_parameters.species_link_min));
    m_species_link_max_edit->setText(
            QString::number(m_parameters.species_link_max));
}

void SettingsWidget::save_operators()
{
    m_current_operators =
            static_cast<size_t>(m_operators_selector->currentIndex());
    check_current_operators();

    m_operators = *get_current_operators();

    QSettings settings;

    settings.setValue(
            "current_operators",
            static_cast<qulonglong>(m_current_operators));

    settings.sync();
}

void SettingsWidget::reset_operators()
{
    check_current_operators();

    QStringList items;
    list_for_each(ConstGAOperatorsPtr, g_plugin_operators, var)
    {
        items.append(list_var_value(var)->name);
    }
    m_operators_selector->clear();
    m_operators_selector->addItems(items);
    m_operators_selector->setCurrentIndex(static_cast<int>(m_current_operators));
}

void SettingsWidget::save_ui_settings()
{
    m_ui_settings.min_fitness =
            m_min_fitness_edit->text().toDouble();
    m_ui_settings.max_fitness =
            m_max_fitness_edit->text().toDouble();
    m_ui_settings.max_species_count =
            m_max_species_count_edit->text().toULongLong();
    m_ui_settings.iterations_buffer_size =
            m_iterations_buffer_size_edit->text().toULongLong();

    QSettings settings;

    settings.setValue(
            "min_fitness",
            m_ui_settings.min_fitness);
    settings.setValue(
            "max_fitness",
            m_ui_settings.max_fitness);
    settings.setValue(
            "max_species_count",
            static_cast<qulonglong>(m_ui_settings.max_species_count));
    settings.setValue(
            "iterations_buffer_size",
            static_cast<qulonglong>(m_ui_settings.iterations_buffer_size));

    settings.sync();
}

void SettingsWidget::reset_ui_settings()
{
    m_min_fitness_edit->setText(
            QString::number(m_ui_settings.min_fitness));
    m_max_fitness_edit->setText(
            QString::number(m_ui_settings.max_fitness));
    m_max_species_count_edit->setText(
            QString::number(m_ui_settings.max_species_count));
    m_iterations_buffer_size_edit->setText(
            QString::number(m_ui_settings.iterations_buffer_size));
}

void SettingsWidget::check_current_objective() const
{
    if (m_current_objective >= list_len(g_plugin_objectives))
    {
        qFatal("Current objective index is out of range: %zu",
               m_current_objective);
    }
}

void SettingsWidget::check_current_operators() const
{
    if (m_current_operators >= list_len(g_plugin_operators))
    {
        qFatal("Current operators index is out of range: %zu",
               m_current_operators);
    }
}

const Objective* SettingsWidget::get_current_objective() const
{
    size_t i = 0;
    list_for_each(ConstObjectivePtr, g_plugin_objectives, var)
    {
        if (i++ == m_current_objective)
        {
            return list_var_value(var);
        }
    }
    qFatal("Current objective index is invalid: %zu", m_current_objective);
    abort();
}

const GAOperators* SettingsWidget::get_current_operators() const
{
    size_t i = 0;
    list_for_each(ConstGAOperatorsPtr, g_plugin_operators, var)
    {
        if (i++ == m_current_operators)
        {
            return list_var_value(var);
        }
    }
    qFatal("Current operators index is invalid: %zu", m_current_operators);
    abort();
}

void SettingsWidget::find_current_objective()
{
    size_t i = 0;
    list_for_each(ConstObjectivePtr, g_plugin_objectives, var)
    {
        if (m_parameters.objective.func == list_var_value(var)->func)
        {
            m_current_objective = i;
            break;
        }
        ++i;
    }
}

void SettingsWidget::find_current_operators()
{
    size_t i = 0;
    list_for_each(ConstGAOperatorsPtr, g_plugin_operators, var)
    {
        if (m_operators.mutation == list_var_value(var)->mutation
            && m_operators.crossover == list_var_value(var)->crossover
            && m_operators.clustering == list_var_value(var)->clustering
            && m_operators.children_selection == list_var_value(var)->children_selection
            && m_operators.selection == list_var_value(var)->selection)
        {
            m_current_operators = i;
            break;
        }
        ++i;
    }
}

//
// Created by Oleg on 11/25/17.
//

#ifndef GUI_SETTINGSWIDGET_H
#define GUI_SETTINGSWIDGET_H

#include <QWidget>

extern "C"
{
#include "GeneticAlgorithm/GAParameters.h"
#include "GeneticAlgorithm/GAObjective.h"
#include "GeneticAlgorithm/GAOperators.h"
};

#include "GraphWidget.h"

class QLineEdit;
class QComboBox;
class QGroupBox;

class SettingsWidget : public QWidget
{
Q_OBJECT

public:
    struct UISettings
    {
        double min_fitness;
        double max_fitness;
        size_t max_species_count;
        size_t iterations_buffer_size;
        GraphWidget::GraphParameters graph_params;
    };

public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    SettingsWidget(const SettingsWidget& settings,
                   QWidget* parent = nullptr);

    const GAParameters& parameters() const;
    const GAOperators* operators() const;
    const UISettings& ui_settings() const;

signals:
    void settings_changed();

public slots:
    void save_settings();
    void reset_settings();

private:
    void init_widget();
    QGroupBox* create_evolution_box();
    QGroupBox* create_operators_box();
    QGroupBox* create_ui_settings_box();

    void save_parameters();
    void reset_parameters();

    void save_operators();
    void reset_operators();

    void save_ui_settings();
    void reset_ui_settings();

    void check_current_objective() const;
    void check_current_operators() const;

    const Objective* get_current_objective() const;
    const GAOperators* get_current_operators() const;

    void find_current_objective();
    void find_current_operators();

private:
    const bool m_read_only;

    GAParameters m_parameters;
    const GAOperators* m_operators;
    UISettings m_ui_settings;

    size_t m_current_objective;
    size_t m_current_operators;

    // parameters
    QLineEdit* m_initial_world_size_edit;
    QLineEdit* m_chromosome_size_edit;
    QLineEdit* m_mutation_probability_edit;
    QLineEdit* m_mutation_on_iteration_dependence_edit;
    QLineEdit* m_crossover_probability_edit;
    QLineEdit* m_selection_worst_probability_edit;
    QLineEdit* m_selection_best_probability_edit;
    QLineEdit* m_selection_elitists_count_edit;
    QLineEdit* m_min_pts_edit;
    QLineEdit* m_eps_edit;
    QComboBox* m_objective_selector;
    QLineEdit* m_max_generations_count_edit;
    QLineEdit* m_stable_value_iterations_count_edit;
    QLineEdit* m_stable_value_eps_edit;
    QLineEdit* m_species_link_iterations_count_edit;
    QLineEdit* m_species_link_probability_edit;
    QLineEdit* m_species_link_min_edit;
    QLineEdit* m_species_link_max_edit;
    QLineEdit* m_scattering_power_edit;
    QLineEdit* m_fitness_influence_power_edit;
    QLineEdit* m_distance_influence_power_edit;
    QLineEdit* m_size_influence_power_edit;
    QLineEdit* m_penalty_power_edit;

    // operators
    QComboBox* m_operators_selector;

    // ui settings
    QLineEdit* m_min_fitness_edit;
    QLineEdit* m_max_fitness_edit;
    QLineEdit* m_max_species_count_edit;
    QLineEdit* m_iterations_buffer_size_edit;
    QLineEdit* m_graph_detalization_level_edit;
    QLineEdit* m_graph_update_progress_times_edit;
};


#endif //GUI_SETTINGSWIDGET_H

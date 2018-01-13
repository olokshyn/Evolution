//
// Created by Oleg on 12/5/15.
//

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "GeneticAlgorithm/GAParameters.h"
#include "GeneticAlgorithm/GAOperators.h"
#include "GeneticAlgorithm/GAObjective.h"
#include "GeneticAlgorithm/GeneticAlgorithm.h"
#include "PluginManager/PluginManager.h"

#include "Logging/Logging.h"

#if COLORED_OUTPUT
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#else
#define ANSI_COLOR_GREEN
#define ANSI_COLOR_RED
#define ANSI_COLOR_RESET
#endif

#define EPS 1e-5

const char* success_template = ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET
        ": best: %.15f "
        "\t iterations: %zu "
        "\t avg time spent on a step: %.5f\n";

void RunForOneAvg(GAParameters* parameters,
                  const GAOperators* operators,
                  size_t tests_count,
                  FILE* report_file) {
    double avg_optimum = 0.0;
    double avg_iterations_made = 0.0;
    double avg_time_spent = 0.0;
    for (size_t test_number = 0;
         test_number != tests_count;
         ++test_number) {
        if (report_file) {
            fprintf(report_file, "Test number %zu\n", test_number + 1);
        }

        GAResult result = RunEvolution(parameters, operators, NULL);

        if (result.error) {
            printf("Error occurred\n");
            return;
        }

        avg_optimum += result.optimum;
        avg_iterations_made += result.iterations_made;
        avg_time_spent += result.time_spent_per_iteration;

        if (report_file) {
            fprintf(report_file,
                    success_template,
                    result.optimum,
                    result.iterations_made,
                    result.time_spent_per_iteration);
            fprintf(report_file, "\n");
            fflush(report_file);
        }
    }
    printf("Avg optimum: %.15f\n", avg_optimum / tests_count);
    printf("Avg iterations made: %.5f\n",
           avg_iterations_made / tests_count);
    printf("Avg time spent: %.5f\n", avg_time_spent);

    printf("\n\n");
    fflush(stdout);
}

void RunForAllFunctions(GAParameters* parameters,
                        const GAOperators* operators,
                        size_t tests_count) {
    FILE* report_file = fopen("RunReport.log", "w");
    if (!report_file) {
        printf("Failed to open RunReport.log\n");
        return;
    }

    list_for_each(ConstObjectivePtr, g_plugin_objectives, var) {
        printf("%s\n", list_var_value(var)->name);
        parameters->objective = list_var_value(var);

        RunForOneAvg(parameters, operators, tests_count, report_file);
    }
    fclose(report_file);
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));

    if (!load_plugins("../Plugins/Objectives", "../Plugins/Operators")) {
        fprintf(stderr, "Failed to load plugins: %s", g_PM_last_error);
        return 1;
    }

    if (!InitLogging("Evolution.log", INFO)) {
        printf("Failed to init logging\n");
        goto error;
    }

#ifndef NDEBUG
    Log(INFO, "Debug configuration");
#else
    Log(INFO, "Release configuration");
#endif

    GAParameters parameters = {
            .initial_world_size = 200,
            .chromosome_size = 25,
            .mutation_probability = 0.2,
            .mutation_on_iteration_dependence = 5.0,
            .crossover_probability = 0.6,
            .selection_worst_probability = 0.5,
            .selection_best_probability = 1.5,
            .selection_elitists_count = 5,
            .min_pts = 1,
            .eps = 0.2,
            .max_generations_count = 500,
            .stable_value_iterations_count = 25,
            .stable_value_eps = 0.001,
            .species_link_iterations_count = 10,
            .species_link_probability = 0.5,
            .species_link_min = -1.0,
            .species_link_max = 1.0,
            .scattering_power = 0.01,
            .fitness_influence_power = 0.5,
            .distance_influence_power = 0.15,
            .size_influence_power = 0.2,
            .penalty_power = 0.75,
    };
    list_for_each(ConstGAOperatorsPtr, g_plugin_operators, var) {
        printf("\n---Operators: %s ---\n", list_var_value(var)->name);
        RunForAllFunctions(&parameters, list_var_value(var), 15);
    }

    ReleaseLogging();
    unload_plugins();

    return 0;

error:
    unload_plugins();
    return 1;
}
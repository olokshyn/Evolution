//
// Created by Oleg on 12/5/15.
//

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "GeneticAlgorithm/GAParameters.h"
#include "GeneticAlgorithm/GAOperators.h"
#include "GeneticAlgorithm/GeneticAlgorithm.h"
#include "Functions/TestFunctions.h"

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
": optimum: %.15f \t best: %.15f "
"\t iterations: %zu "
"\t avg time spent on a step: %.5f\n";

const char* failure_template = ANSI_COLOR_RED "FAILURE" ANSI_COLOR_RESET
": optimum: %.15f \t best: %.15f "
"\t iterations: %zu "
"\t avg time spent on a step: %.5f\n";

void RunForOneAvg(GAParameters* parameters,
                  GAOperators* operators,
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

        GAResult result = RunEvolution(parameters, operators);

        if (result.error) {
            printf("Error occurred\n");
            return;
        }

        avg_optimum += result.optimum;
        avg_iterations_made += result.iterations_made;
        avg_time_spent += result.time_spent_per_iteration;

        if (fabs(parameters->objective.optimum - result.optimum) < EPS) {
            if (report_file) {
                fprintf(report_file,
                        success_template,
                        parameters->objective.optimum,
                        result.optimum,
                        result.iterations_made,
                        result.time_spent_per_iteration);
            }
        }
        else {
            if (report_file) {
                fprintf(report_file,
                        failure_template,
                        parameters->objective.optimum,
                        result.optimum,
                        result.iterations_made,
                        result.time_spent_per_iteration);
            }
        }

        if (report_file) {
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
                        GAOperators* operators,
                        size_t tests_count) {
    const char* functionNames[] = {
            "De Jong`s F1 function",
            "De Jong`s F2 function",
            "De Jong`s F3 function",
            "De Jong`s F4 function",
            "De Jong`s F5 function",
            "Rastrigin`s function",
            "Schwefel`s function",
            "Griewangk`s function",
            "Stretched V sine wave function",
            "Ackley`s function",
            "EggHolder function",
            "Pathological test function"
    };

    Objective objectiveFunctions[] = {
            DeJongF1Objective,
            DeJongF2Objective,
            DeJongF3Objective,
            DeJongF4Objective,
            DeJongF5Objective,
            RastriginFuncObjective,
            SchwefelFuncObjective,
            GriewangkFuncObjective,
            StretchedVSineWaveFuncObjective,
            AckleyFuncObjective,
            EggHolderFuncObjective,
            PathologicalFuncObjective
    };

    LOG_ASSERT(sizeof(functionNames) / sizeof(char*)
               == sizeof(objectiveFunctions) / sizeof(Objective));

    size_t functionsCount = sizeof(functionNames) / sizeof(char*);

    FILE* report_file = fopen("RunReport.log", "w");
    if (!report_file) {
        printf("Failed to open RunReport.log\n");
        return;
    }

    for (size_t i = 0; i != functionsCount; ++i) {
        printf("%s\n", functionNames[i]);
        parameters->objective = objectiveFunctions[i];

        RunForOneAvg(parameters, operators, tests_count, report_file);
    }
}

void RunForAllHerreraFunctions(GAParameters* parameters,
                               GAOperators* operators,
                               size_t tests_count) {
    const char* functionNames[] = {
            "f1 - De Jong`s F1 function",
            "f2 - De Jong`s F2 function",
            "f3 - Rastrigin`s function",
            "f4 - Ackley`s function",
            "f6 - Griewangk`s function"
    };

    Objective objectiveFunctions[] = {
            DeJongF1Objective,
            DeJongF2Objective,
            RastriginFuncObjective,
            AckleyFuncObjective,
            GriewangkFuncObjective
    };

    LOG_ASSERT(sizeof(functionNames) / sizeof(char*)
               == sizeof(objectiveFunctions) / sizeof(Objective));

    size_t functionsCount = sizeof(functionNames) / sizeof(char*);

    FILE* report_file = fopen("RunReport.log", "w");
    if (!report_file) {
        printf("Failed to open RunReport.log\n");
        return;
    }

    for (size_t i = 0; i != functionsCount; ++i) {
        printf("%s\n", functionNames[i]);
        fprintf(report_file, "%s\n", functionNames[i]);
        parameters->objective = objectiveFunctions[i];

        RunForOneAvg(parameters, operators, tests_count, report_file);
    }
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));

    if (!InitLogging("Evolution.log", INFO)) {
        printf("Failed to init logging\n");
        return 1;
    }

#ifndef NDEBUG
    Log(INFO, "Debug configuration");
#else
    Log(INFO, "Release configuration");
#endif

    GAParameters parameters = {
            .initial_world_size = 61,
            .chromosome_size = 25,
            .mutation_probability = 0.6,
            .mutation_on_iteration_dependence = 5.0,
            .crossover_probability = 0.6,
            .selection_worst_probability = 0.5,
            .selection_best_probability = 1.5,
            .selection_elitists_count = 5,
            .k = 5,
            .h = 0.0,
            .max_generations_count = 5000,
            .stable_value_iterations_count = 0,
            .stable_value_eps = 1e-5,
    };

    GAOperators operators[] = {
            HERRERA_GA_OPERATORS,
            HERRERA_WITH_CLUSTERING_GA_OPERATORS,
            LOKSHYN_GA_OPERATORS
    };

    const char* operators_names[] = {
            "Herrera GA",
            "Herrera with clustering GA",
            "Lokshyn GA"
    };

    LOG_ASSERT(sizeof(operators) / sizeof(operators[0])
               == sizeof(operators_names) / sizeof(operators_names[0]));
    size_t operators_count = sizeof(operators) / sizeof(operators[0]);

    for (size_t i = 0; i != operators_count; ++i) {
        printf("\n---Operators: %s ---\n", operators_names[i]);
        RunForAllFunctions(&parameters, &operators[i], 15);
    }

    ReleaseLogging();

    return 0;
}
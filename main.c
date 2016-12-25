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

#define EPS 0.001

const char* success_template = ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET
": optimum: %.3f \t best: %.3f "
"\t iterations: %zu "
"\t avg time spent on a step: %.3f\n";

const char* failure_template = ANSI_COLOR_RED "FAILURE" ANSI_COLOR_RESET
": optimum: %.3f \t best: %.3f "
"\t iterations: %zu "
"\t avg time spent on a step: %.3f\n";

void RunForAllFunctions(GAParameters* parameters,
                        GAOperators* operators) {
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

    for (size_t i = 0; i != functionsCount; ++i) {
        printf("%s\n", functionNames[i]);
        parameters->objective = objectiveFunctions[i];

        GAResult result = RunEvolution(parameters, operators);

        if (result.error) {
            printf("Error occurred\n");
            return;
        }

        if (fabs(objectiveFunctions[i].optimum - result.optimum) < EPS) {
            printf(success_template,
                   objectiveFunctions[i].optimum,
                   result.optimum,
                   result.iterations_made,
                   result.time_spent_per_iteration);
        }
        else {
            printf(failure_template,
                   objectiveFunctions[i].optimum,
                   result.optimum,
                   result.iterations_made,
                   result.time_spent_per_iteration);
        }

        printf("\n");
    }
}

void RunForOneAvg(GAParameters* parameters,
                  GAOperators* operators,
                  size_t tests_count) {
    double avg_optimum = 0.0;
    double avg_iterations_count = 0.0;
    double avg_time_spent = 0.0;
    size_t success_iterations_count = 0;
    for (size_t i = 0; i != tests_count; ++i) {

        GAResult result = RunEvolution(parameters, operators);

        if (result.error) {
            printf("Error occured\n");
            return;
        }

        if (fabs(parameters->objective.optimum - result.optimum) < EPS) {
            printf(success_template,
                   parameters->objective.optimum,
                   result.optimum,
                   result.iterations_made,
                   result.time_spent_per_iteration);

            ++success_iterations_count;
        }
        else {
            printf(failure_template,
                   parameters->objective.optimum,
                   result.optimum,
                   result.iterations_made,
                   result.time_spent_per_iteration);
        }

        avg_optimum += result.optimum;
        avg_iterations_count += result.iterations_made;
        avg_time_spent += result.time_spent_per_iteration;
    }

    avg_optimum /= tests_count;
    avg_iterations_count /= tests_count;
    avg_time_spent /= tests_count;

    printf("Avg optimum: %.3f\n"
                   "Avg iterations made: %.3f\n"
                   "Avg time spent: %.3f\n",
           avg_optimum, avg_iterations_count, avg_time_spent);
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


    // runAllListTests();
    // four_points_test();
    // eight_points_test();
    // twelve_points_test();
    // fisher_iris_test();

    GAParameters parameters = {
            .initial_world_size = 61,
            .chromosome_size = 25,
            .mutation_probability = 0.125,
            .mutation_on_iteration_dependence = 1.0,
            .k = 5,
            .h = 0.0,
            .objective = SchwefelFuncObjective,
            .max_generations_count = 100,
            .stable_value_iterations_count = 100,
            .stable_value_eps = 1e-5,
            .worst_selection_probability = 0.5,
            .best_selection_probability = 1.5
    };

    GAOperators operators = HERRERA_GA_OPERATORS;

//    RunForOneAvg(&parameters, &operators, 1);
    RunForAllFunctions(&parameters, &operators);


    ReleaseLogging();

    return 0;
}
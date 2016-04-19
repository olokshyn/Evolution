#include "Simulation.h"

#define COLORED_OUTPUT 0

#if COLORED_OUTPUT
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_RESET   "\x1b[0m"
#else
    #define ANSI_COLOR_GREEN   "\t"
    #define ANSI_COLOR_RED
    #define ANSI_COLOR_RESET
#endif


const char* success_template = ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET
                               ": optimum: %.3f \t best: %.3f "
                               "\t iterations: %zu\n";
const char* failure_template = ANSI_COLOR_RED "FAILURE" ANSI_COLOR_RESET
                               ": optimum: %.3f \t best: %.3f "
                               "\t iterations: %zu\n";
const char* complete_template = "optimum: %.3f \t best: %.3f "
                                "\t iterations: %zu\n";

int RunSimulation(size_t max_iterations_count,
                  size_t stable_value_iterations_count,
                  double stable_value_eps,
                  size_t individuals_count,
                  size_t chromosome_size,
                  double mutation_probability,
                  size_t k_neighbour,
                  double cluster_height,
                  Objective objective,
                  size_t* iterations_made,
                  short silent) {
    int k = 0;
    double max_fitness = 0, cur_fitness = 0, prev_max_fitness = 0;
    World world;
    CreateWorld(&world,
                individuals_count,
                chromosome_size,
                mutation_probability,
                k_neighbour,
                cluster_height,
                objective);
    if (GetLastError()) {
        printf("Error creating world\n");
        goto error_RunSimulation;
    }
    max_fitness = GetMaxFitness(&world);
    // printf("Initial fitness: %.3f\n\n", max_fitness);

    size_t value_is_stable_count = 0;
    for (size_t i = 0; i < max_iterations_count; ++i) {
        cur_fitness = Step(&world);
        if (GetLastError()) {
            printf("Error occurred, terminating...\n");
            goto error_RunSimulation;
        }
        ++k;
        if (cur_fitness - max_fitness > 0.001) {
            // printf("%d\t%d\t%.3f\n", k, i, cur_fitness);
            k = 0;
            max_fitness = cur_fitness;
        }
        if (fabs(max_fitness - prev_max_fitness) < stable_value_eps) {
            ++value_is_stable_count;
            if (stable_value_iterations_count > 0
                    && value_is_stable_count >= stable_value_iterations_count) {
                int success = 1;
                if (!silent) {
                    if (fabs(objective.optimum - max_fitness) < 0.001) {
                        printf(success_template,
                               objective.optimum,
                               max_fitness,
                               i);
                        success = 1;
                    }
                    else {
                        printf(failure_template,
                               objective.optimum,
                               max_fitness,
                               i);
                        success = 0;
                    }
                }
                ClearWorld(&world);
                if (iterations_made) {
                    *iterations_made = i;
                }
                return success;
            }
        }
        else {
            value_is_stable_count = 0;
        }
        prev_max_fitness = max_fitness;
    }
    if (!silent) {
        if (stable_value_iterations_count > 0) {
            printf(failure_template,
                   objective.optimum,
                   max_fitness,
                   max_iterations_count);
        }
        else {
            printf(complete_template,
                   objective.optimum,
                   max_fitness,
                   max_iterations_count);
        }
    }
    ClearWorld(&world);
    if (iterations_made) {
        *iterations_made = max_iterations_count;
    }
    return 0;

error_RunSimulation:
    printf(failure_template,
           objective.optimum,
           max_fitness,
           max_iterations_count);
    ClearWorld(&world);
    if (iterations_made) {
        *iterations_made = max_iterations_count;
    }
    return 0;
}
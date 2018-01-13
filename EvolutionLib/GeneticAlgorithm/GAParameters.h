//
// Created by Oleg on 9/27/16.
//

#ifndef EVOLUTION_GAPARAMETERS_H
#define EVOLUTION_GAPARAMETERS_H

#include "GAFwd.h"
#include "Common.h"
#include "GAObjective.h"

#define ENABLE_THREADS 0
#if defined(ENABLE_THREADS) && ENABLE_THREADS
    #define THREADS_COUNT 4
#endif

#define COLORED_OUTPUT 0

#define CROSSOVER_FIRST_TIME_PROB 0.1
#define CROSSOVER_EXTINCTION_BIAS 8
#define CROSSOVER_MIN_PROB 0.2
#define CROSSOVER_MAX_PROB 0.8
#define CROSSOVER_ALPHA 1.0

#define EXTINCTION_BIAS 0.9

typedef struct ga_parameters {
    size_t initial_world_size;
    size_t chromosome_size;
    double mutation_probability;
    double mutation_on_iteration_dependence;
    double crossover_probability;
    double selection_worst_probability;
    double selection_best_probability;
    size_t selection_elitists_count;
    size_t min_pts;
    double eps;
    const Objective* objective;
    size_t max_generations_count;
    size_t stable_value_iterations_count;
    double stable_value_eps;
    size_t species_link_iterations_count;
    double species_link_probability;
    double species_link_min;
    double species_link_max;
    double scattering_power;
    double fitness_influence_power;
    double distance_influence_power;
    double size_influence_power;
    double penalty_power;
} GAParameters;

inline static size_t get_chromosome_size(const GAParameters* parameters) {
    size_t chr_size = parameters->chromosome_size;
    if (parameters->objective->max_args_count != 0) {
        chr_size = MIN(chr_size, parameters->objective->max_args_count);
    }
    return chr_size;
}

#endif //EVOLUTION_GAPARAMETERS_H

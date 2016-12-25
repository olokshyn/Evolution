//
// Created by Oleg on 9/27/16.
//

#ifndef EVOLUTION_GAPARAMETERS_H
#define EVOLUTION_GAPARAMETERS_H

#include "GAFwd.h"
#include "Common.h"

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

#define SELECTION_MIN 0.0
#define SELECTION_MAX 1.0

#define EXTINCTION_BIAS 0.9

#define SPECIES_LINK_PROBABILITY 0.5
#define SPECIES_LINK_MIN -1.0
#define SPECIES_LINK_MAX  1.0

typedef struct ga_parameters {
    size_t initial_world_size;
    size_t chromosome_size;
    double mutation_probability;
    double mutation_on_iteration_dependence;
    double crossover_probability;
    size_t k;
    double h;
    Objective objective;
    size_t max_generations_count;
    size_t stable_value_iterations_count;
    double stable_value_eps;
    double worst_selection_probability;
    double best_selection_probability;
} GAParameters;

#endif //EVOLUTION_GAPARAMETERS_H

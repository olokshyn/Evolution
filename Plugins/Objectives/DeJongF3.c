//
// Created by oleg on 16.12.17.
//

#include "GeneticAlgorithm/GAObjective.h"
#include "Logging/Logging.h"

#define MAX_ARGS_COUNT 5

double DeJongF3(const double* args, size_t args_size) {
    LOG_ASSERT(args_size <= MAX_ARGS_COUNT);
    double res = 0;
    for (int i = 0; i < args_size; ++i) {
        res += (int)args[i];  // get the integer part of Number
    }
    return res;
}

const Objective objective = {
        .func = DeJongF3,
        .min = -5.12,
        .max = 5.12,
        .max_args_count = MAX_ARGS_COUNT,
        .name = "De Jong F3"
};

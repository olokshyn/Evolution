//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"
#include "Logging/Logging.h"

#define MAX_ARGS_COUNT 10

double Griewangk(const double* args, size_t args_size) {
    LOG_ASSERT(args_size <= MAX_ARGS_COUNT);
    double res = 0;
    for (int i = 0; i < args_size; ++i) {
        res += args[i] * args[i];
    }
    res = res / 4000 + 1;
    double temp = 1;
    for (int i = 0; i < args_size; ++i) {
        temp *= cos(args[i] / sqrt((double)(i + 1)));
    }
    res -= temp;
    return -res;
}

const Objective objective = {
        .func = Griewangk,
        .min = -600,
        .max = 600,
        .max_args_count = MAX_ARGS_COUNT,
        .name = "Griewangk"
};

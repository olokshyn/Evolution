//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"
#include "Logging/Logging.h"

#define MAX_ARGS_COUNT 2

double EggHolder(double* args, size_t args_size) {
    LOG_ASSERT(args_size <= MAX_ARGS_COUNT);
    double res = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        res -= args[i] * sin(sqrt(fabs(args[i] - args[i + 1] - 47)))
               + (args[i + 1] + 47)
                 * sin(sqrt(fabs(args[i + 1] + 47 + args[i] / 2.0)));
    }
    return -res;
}

const Objective objective = {
        .func = EggHolder,
        .min = -512,
        .max = 512,
        .max_args_count = MAX_ARGS_COUNT,
        .name = "Egg Holder"
};

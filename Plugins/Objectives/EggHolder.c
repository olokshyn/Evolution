//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double EggHolder(double* args, size_t args_size) {
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
        .max_args_count = 2,
        .name = "Egg Holder"
};

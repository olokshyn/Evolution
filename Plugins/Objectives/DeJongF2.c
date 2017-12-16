//
// Created by oleg on 14.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double DeJongF2(double* args, size_t args_size) {
    double res = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        res += 100 * pow(args[i + 1] - args[i] * args[i], 2.0)
               + pow(1 - args[i], 2.0);
    }
    return -res;
}

const Objective objective = {
        .func = DeJongF2,
        .min = -5.12,
        .max = 5.12,
        .name = "De Jong F2"
};

//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"
#include "Common.h"

double DeJongF4(const double* args, size_t args_size) {
    double res = getRand(0, 1);  // TODO: Gauss distribution must be used
    for (int i = 0; i < args_size; ++i) {
        res += (i + 1) * pow(args[i], 4.0);
    }
    return -res;
}

const Objective objective = {
        .func = DeJongF4,
        .min = -1.28,
        .max = 1.28,
        .name = "De Jong F4"
};

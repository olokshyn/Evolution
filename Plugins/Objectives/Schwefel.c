//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double Schwefel(const double* args, size_t args_size) {
    double res = 418.9829 * args_size;
    for (int i = 0; i < args_size; ++i) {
        res += -args[i] * sin(sqrt(fabs(args[i])));
    }
    return -res;
}

const Objective objective = {
        .func = Schwefel,
        .min = -500,
        .max = 500,
        .name = "Schwefel"
};

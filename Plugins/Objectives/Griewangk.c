//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double Griewangk(double* args, size_t args_size) {
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
        .max_args_count = 10,
        .name = "Griewangk"
};

//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double Rastrigin(double* args, size_t args_size) {
    double res = 10 * args_size;
    for (int i = 0; i < args_size; ++i) {
        res += args[i] * args[i] - 10 * cos(2 * M_PI * args[i]);
    }
    return -res;
}

const Objective objective = {
        .func = Rastrigin,
        .min = -5.12,
        .max = 5.12,
        .name = "Rastrigin"
};

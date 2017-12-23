//
// Created by oleg on 14.12.17.
//

#include "GeneticAlgorithm/GAObjective.h"

double DeJongF1(const double* args, size_t args_size) {
    double res = 0;
    for (int i = 0; i < args_size; ++i) {
        res += args[i] * args[i];
    }
    return -res;
}

const Objective objective = {
        .func = DeJongF1,
        .min = -5.12,
        .max = 5.12,
        .name = "De Jong F1"
};

//
// Created by oleg on 16.12.17.
//

#include "GeneticAlgorithm/GAObjective.h"

double DeJongF3(double* args, size_t args_size) {
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
        .max_args_count = 5,
        .name = "De Jong F3"
};

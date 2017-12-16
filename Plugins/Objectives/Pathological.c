//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double Pathological(double* args, size_t args_size) {
    double res = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        // TODO: check this function
        // double temp = 1 + 0.001 * pow(args[i] - args[i + 1], 4);
        res += 0.5
               + (pow(sin(sqrt(100 * pow(args[i], 2) + pow(args[i + 1], 2))), 2) - 0.5)
                 / (1 + 0.001 * pow(args[i] - args[i + 1], 4));
    }
    return -res;
}

const Objective objective = {
        .func = Pathological,
        .min = -100,
        .max = 100,
        .name = "Pathological"
};

//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double StretchedVSineWave(const double* args, size_t args_size) {
    double res = 0, t = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        t = pow(args[i], 2) + pow(args[i + 1], 2);
        res += pow(t, 0.25)
               * pow(sin(50 * pow(t, 0.1)), 2) + 1;
    }
    return -res;
}

const Objective objective = {
        .func = StretchedVSineWave,
        .min = -100,
        .max = 100,
        .name = "Stretched V Sine Wave"
};

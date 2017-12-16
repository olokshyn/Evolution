//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double Rana(double* args, size_t args_size) {
    double res = 0, t1 = 0, t2 = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        t1 = sqrt(fabs(args[i + 1] + 1 - args[i]));
        t2 = sqrt(fabs(args[i + 1] + 1 + args[i]));
        res += args[i]
               * sin(t1)
               * cos(t2)
               + (args[i + 1] + 1)
                 * cos(t1)
                 * sin(t2);
    }
    return res;
}

const Objective objective = {
        .func = Rana,
        .min = -500,
        .max = 500,
        .name = "Rana"
};

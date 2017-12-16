//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double Ackley(double* args, size_t args_size) {
    double res = 0, t1 = 0, t2 = 0;
    for (int i = 0; i < args_size; ++i) {
        t1 += args[i] * args[i];
        t2 += cos(2 * M_PI * args[i]);
    }
    res = -20 * exp(-0.2 * sqrt(1.0 / args_size * t1))
          - exp(1.0 / args_size * t2) + 20 + M_E;
    //    for (int i = 0; i < args_size - 1; ++i) {
    //        res += 20 + M_E
    //            - 20 * pow(M_E, -0.2 * sqrt((pow(args[i], 2) + pow(args[i + 1], 2)) / 2))
    //            - pow(M_E, 0.5 * (cos(2 * M_PI * args[i]) + cos(2 * M_PI + args[i + 1])));
    //    }
    return -res;
}

const Objective objective = {
        .func = Ackley,
        .min = -32.768,
        .max = 32.768,
        .name = "Ackley"
};

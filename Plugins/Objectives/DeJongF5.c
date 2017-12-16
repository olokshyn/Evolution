//
// Created by oleg on 16.12.17.
//

#include <math.h>

#include "GeneticAlgorithm/GAObjective.h"

double DeJongF5(double* args, size_t args_size) {
    double res = 0;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; j++) {
            res += 1 / (5*(i + 2) + j + 3 +
                        pow(args[0] - 16*j, 6.0) + pow(args[1] - 16*i, 6.0));
        }
    }
    res += 0.002;
    return 1 / res;
}

const Objective objective = {
        .func = DeJongF5,
        .min = -65.536,
        .max = 65.536,
        .max_args_count = 2,
        .name = "De Jong F5"
};

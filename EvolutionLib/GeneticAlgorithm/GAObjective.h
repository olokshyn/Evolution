//
// Created by oleg on 14.12.17.
//

#ifndef EVOLUTION_GAOBJECTIVE_H
#define EVOLUTION_GAOBJECTIVE_H

#include <stddef.h>

typedef double (*ObjectiveFunc)(const double* args, size_t args_count);

typedef struct objective {
    ObjectiveFunc func;
    double min;
    double max;
    size_t max_args_count;
    const char* name;
} Objective;

#endif //EVOLUTION_GAOBJECTIVE_H

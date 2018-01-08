//
// Created by Oleg on 12/24/16.
//

#include "GAOperators.h"

#include <math.h>
#include <float.h>

#include "World.h"
#include "GAParameters.h"

bool GAO_ConvergenceStopIterationHook(World* world, size_t generation_number, double max_fitness) {
    static _Thread_local size_t stable_iterations_done = 0;
    static _Thread_local double last_max_fitness = -DBL_MAX;

    if (fabs(max_fitness - last_max_fitness) < world->parameters->stable_value_eps) {
        ++stable_iterations_done;
    }
    else {
        stable_iterations_done = 0;
        last_max_fitness = max_fitness;
    }
    if (stable_iterations_done >= world->parameters->stable_value_iterations_count) {
        return true;
    }
    return false;
}

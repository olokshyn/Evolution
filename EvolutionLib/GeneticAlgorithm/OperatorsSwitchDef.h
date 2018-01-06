//
// Created by oleg on 06.01.18.
//

#ifndef EVOLUTION_OPERATORSSWITCHDEF_H
#define EVOLUTION_OPERATORSSWITCHDEF_H

#include <math.h>
#include <float.h>

#include "World.h"
#include "GAParameters.h"

#ifndef STOP_ON_CONVERGENCE
#define STOP_ON_CONVERGENCE true
#endif

bool GAO_SwitchIterationInfoHook(World* world, size_t generation_number, double max_fitness) {
    static _Thread_local size_t switch_to_index = 0;
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
        if (switch_to_index < SWITCH_OPERATORS_COUNT) {
            const char* name = world->operators->name;
            iteration_info_hook_t info_hook = world->operators->iteration_info_hook;

            *world->operators = *switch_to_operators_ptrs[switch_to_index++];
            world->operators->name = name;
            world->operators->iteration_info_hook = info_hook;

            stable_iterations_done = 0;
        }
        else {
            return STOP_ON_CONVERGENCE;
        }
    }
    return false;
}

#endif //EVOLUTION_OPERATORSSWITCHDEF_H

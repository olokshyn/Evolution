//
// Created by oleg on 06.01.18.
//

#ifndef EVOLUTION_OPERATORSSWITCH_H
#define EVOLUTION_OPERATORSSWITCH_H

#define REGISTER_SWITCH_TO_OPERATORS(...) \
    static _Thread_local const GAOperators* const switch_to_operators_ptrs[] = {__VA_ARGS__};

#define SWITCH_OPERATORS_COUNT (sizeof(switch_to_operators_ptrs) / sizeof(switch_to_operators_ptrs[0]))

bool GAO_SwitchIterationInfoHook(World* world, size_t generation_number, double max_fitness);

#endif //EVOLUTION_OPERATORSSWITCH_H

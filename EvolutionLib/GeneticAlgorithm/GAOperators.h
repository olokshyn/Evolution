//
// Created by Oleg on 12/24/16.
//

#ifndef EVOLUTION_GAOPERATORS_H
#define EVOLUTION_GAOPERATORS_H

#include <stdbool.h>

#include "GAFwd.h"
#include "Species/Species.h"

typedef bool (*iteration_info_hook_t)(World* world, size_t generation_number, double max_fitness);

typedef struct ga_operators {
    bool (*mutation)(World* world, size_t generation_number);
    LIST_TYPE(EntityPtr) (*crossover)(World* world, size_t generation_number);
    LIST_TYPE(SpeciesPtr) (*clustering)(World* world,
                                        LIST_TYPE(EntityPtr) new_entities,
                                        double eps,
                                        size_t min_pts);
    bool (*children_selection)(World* world, LIST_TYPE(EntityPtr)* new_entities);
    bool (*selection)(World* world);
    iteration_info_hook_t iteration_info_hook;
    const char* name;
} GAOperators;

bool GAO_UniformMutation(World* world, size_t generation_number);

bool GAO_NonUniformMutation(World* world, size_t generation_number);

LIST_TYPE(EntityPtr) GAO_UniformCrossover(World* world,
                                          size_t generation_number);

LIST_TYPE(EntityPtr) GAO_FitnessCrossover(World* world,
                                          size_t generation_number);

LIST_TYPE(SpeciesPtr) GAO_Clustering(World* world,
                                     LIST_TYPE(EntityPtr) new_entities,
                                     double eps,
                                     size_t min_pts);

bool GAO_ChildrenSelection(World* world, LIST_TYPE(EntityPtr)* new_entities);

bool GAO_SpeciesRandomLinksSelection(World* world);

bool GAO_LinearRankingSelection(World* world);

bool GAO_ElitistsSelection(World* world);

bool GAO_ConvergenceStopIterationHook(World* world, size_t generation_number, double max_fitness);

#endif //EVOLUTION_GAOPERATORS_H

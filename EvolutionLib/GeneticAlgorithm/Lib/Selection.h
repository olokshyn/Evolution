//
// Created by oleg on 08.01.18.
//

#ifndef EVOLUTION_SELECTION_H
#define EVOLUTION_SELECTION_H

#include <stdbool.h>

#include "3rdParty/CList/list.h"

#include "GeneticAlgorithm/GAFwd.h"
#include "Species/Species.h"

#ifndef LIST_DEFINED_DOUBLE
DEFINE_LIST(double)
#define LIST_DEFINED_DOUBLE
#endif

bool Selection_Template_FitnessBased(
        World* world,
        bool (*selection_entities)(const World* world,
                                   LIST_TYPE(EntityPtr)* entities_ptr,
                                   size_t alive_count,
                                   size_t* entities_died),
        bool (*adjust_fitnesses)(const World* world,
                                 LIST_TYPE(double) fitnesses));

bool Selection_Entities_Linear(const World* world,
                               LIST_TYPE(EntityPtr)* entities_ptr,
                               size_t alive_count,
                               size_t* entities_died);

bool Selection_Entities_LinearRanking(const World* world,
                                      LIST_TYPE(EntityPtr)* entities_ptr,
                                      size_t alive_count,
                                      size_t* entities_died);

bool Selection_Entities_Linear_Best_Worst(const World* world,
                                          LIST_TYPE(EntityPtr)* entities_ptr,
                                          size_t alive_count,
                                          size_t* entities_died);

bool Selection_AdjustFitnesses_RandomSpeciesLinks(const World* world,
                                                  LIST_TYPE(double) fitnesses);

bool Selection_AdjustFitnesses_SpeciesSizePenalty(const World* world,
                                                  LIST_TYPE(double) fitnesses);

bool Selection_Linear(World* world);

bool Selection_Linear_RandomSpeciesLinks(World* world);

bool Selection_Linear_SpeciesSizePenalty(World* world);

bool Selection_Linear_BestWorstPenalty(World* world);

bool Selection_LinearRanking(World* world);

bool Selection_Children_Linear(World* world, LIST_TYPE(EntityPtr)* new_entities);

#endif //EVOLUTION_SELECTION_H

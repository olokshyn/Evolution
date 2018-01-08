//
// Created by oleg on 08.01.18.
//

#ifndef EVOLUTION_CROSSOVER_H
#define EVOLUTION_CROSSOVER_H

#include <stdbool.h>

#include "3rdParty/CList/list.h"

#include "GeneticAlgorithm/GAFwd.h"
#include "GeneticAlgorithm/GAObjective.h"
#include "Species/Species.h"

#ifndef LIST_DEFINED_DOUBLE
DEFINE_LIST(double)
#define LIST_DEFINED_DOUBLE
#endif

bool Crossover_Entities_Template_Probabilistic(
        const World* world,
        LIST_TYPE(EntityPtr) entities,
        LIST_TYPE(EntityPtr) new_entities,
        double probability,
        size_t generation_number,
        bool (* crossover)(const Entity* parent1,
                           const Entity* parent2,
                           Entity* child1,
                           Entity* child2,
                           const Objective* obj,
                           double fitness1,  // normalized
                           double fitness2,  // normalized
                           size_t chromosome_size,
                           size_t generation_number,
                           size_t max_generations_count));

bool Crossover_Entities_OnePoint(const World* world,
                                 LIST_TYPE(EntityPtr) entities,
                                 LIST_TYPE(EntityPtr) new_entities,
                                 double probability,
                                 size_t generation_number);

bool Crossover_Entities_DHX(const World* world,
                            LIST_TYPE(EntityPtr) entities,
                            LIST_TYPE(EntityPtr) new_entities,
                            double probability,
                            size_t generation_number);

LIST_TYPE(EntityPtr) Crossover_Template(
        World* world,
        size_t generation_number,
        bool (*crossover_entities)(const World* world,
                                   LIST_TYPE(EntityPtr) entities,
                                   LIST_TYPE(EntityPtr) new_entities,
                                   double probability,
                                   size_t generation_number));

LIST_TYPE(EntityPtr) Crossover_OnePoint(World* world,
                                        size_t generation_number);

LIST_TYPE(EntityPtr) Crossover_DHX(World* world,
                                   size_t generation_number);

#endif //EVOLUTION_CROSSOVER_H

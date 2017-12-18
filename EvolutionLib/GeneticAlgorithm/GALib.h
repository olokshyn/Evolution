//
// Created by Oleg on 9/27/16.
//

#ifndef EVOLUTION_GALIB_H
#define EVOLUTION_GALIB_H

#include <stdbool.h>

#include "3rdParty/CList/list.h"

#include "GAFwd.h"
#include "Species/Species.h"

#ifndef LIST_DEFINED_DOUBLE
DEFINE_LIST(double)
#define LIST_DEFINED_DOUBLE
#endif

// Mutation

double NonUniformMutationDelta(size_t t, double y,
                               const GAParameters* parameters);

// Crossover

bool CrossEntitiesWithProbability(World* world,
                                  LIST_TYPE(EntityPtr) entities,
                                  LIST_TYPE(EntityPtr) new_entities,
                                  double probability,
                                  size_t generation_number);

bool CountSpeciesLinks(LIST_TYPE(double) fitnesses);


// Selection

bool FitnessBasedSelectionTemplate(
        World* world,
        bool (*selection)(World* world,
                          LIST_TYPE(EntityPtr)* entities_ptr,
                          size_t alive_count,
                          size_t* entities_died),
        bool (*adjust_fitnesses)(LIST_TYPE(double) fitnesses));

bool LinearRankingSelection(World* world,
                            LIST_TYPE(EntityPtr)* entities_ptr,
                            size_t alive_count,
                            size_t* entities_died);

bool PerformSelectionInEntities(World* world,
                                LIST_TYPE(EntityPtr)* entities_ptr,
                                size_t alive_count,
                                size_t* entities_died);

// Other

double ScaleEps(World* world,
                LIST_TYPE(EntityPtr) new_entities,
                double eps);

bool GaussSLE(double** matrix, size_t rows, size_t cols, double* solution);

#endif //EVOLUTION_GALIB_H

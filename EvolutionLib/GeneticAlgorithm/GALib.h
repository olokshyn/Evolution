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

bool FitnessBasedSelectionTemplate(World* world, bool (*selection)(World* world,
                                                                   LIST_TYPE(EntityPtr)* entities_ptr,
                                                                   size_t alive_count,
                                                                   size_t* entities_died));

bool LinearRankingSelection(World* world,
                            LIST_TYPE(EntityPtr)* entities_ptr,
                            size_t alive_count,
                            size_t* entities_died);

bool PerformSelectionInEntities(World* world,
                                LIST_TYPE(EntityPtr)* entities_ptr,
                                size_t alive_count,
                                size_t* entities_died);

bool PerformLimitedSelectionInSpecies(World* world,
                                      Species* species,
                                      double norm_fitness);

// Other

double ScaleEps(World* world,
                LIST_TYPE(EntityPtr) new_entities,
                double eps);

#endif //EVOLUTION_GALIB_H

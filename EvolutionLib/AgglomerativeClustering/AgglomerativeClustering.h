//
// Created by Oleg on 3/13/16.
//

#ifndef EVOLUTION_AGGLOMERATIVECLUSTERING_H
#define EVOLUTION_AGGLOMERATIVECLUSTERING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GeneticAlgorithm/World.h"
#include "Species/Species.h"

#define MAX_CLUSTER_COUNT 3

LIST_TYPE(SpeciesPtr) AgglomerativeClustering(World* world,
                                              LIST_TYPE(EntityPtr) new_entities,
                                              double eps,
                                              size_t min_pts);

#ifdef __cplusplus
}
#endif

#endif //EVOLUTION_AGGLOMERATIVECLUSTERING_H

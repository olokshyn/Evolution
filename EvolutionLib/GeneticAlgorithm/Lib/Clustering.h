//
// Created by oleg on 08.01.18.
//

#ifndef EVOLUTION_CLUSTERING_H
#define EVOLUTION_CLUSTERING_H

#include "GeneticAlgorithm/GAFwd.h"
#include "Species/Species.h"

LIST_TYPE(SpeciesPtr) Clustering_Template(
        World* world,
        LIST_TYPE(EntityPtr) new_entities,
        double eps,
        size_t min_pts,
        LIST_TYPE(SpeciesPtr) (*clustering)(World* world,
                                            LIST_TYPE(EntityPtr) new_entities,
                                            double eps, size_t min_pts,
                                            LIST_TYPE(EntityPtr) noise_entities));

LIST_TYPE(SpeciesPtr) Clustering_OPTICS(World* world,
                                        LIST_TYPE(EntityPtr) new_entities,
                                        double eps,
                                        size_t min_pts);

#endif //EVOLUTION_CLUSTERING_H

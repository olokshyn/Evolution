//
// Created by Oleg on 10/5/17.
//

#ifndef EVOLUTION_OPTICS_H
#define EVOLUTION_OPTICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GeneticAlgorithm/GAFwd.h"
#include "Species/Species.h"

LIST_TYPE(SpeciesPtr) OPTICSClustering(World* world,
                                       LIST_TYPE(EntityPtr) new_entities,
                                       double eps, size_t min_pts,
                                       LIST_TYPE(EntityPtr) noise_entities);

#ifdef __cplusplus
};

std::vector<size_t> optics_clustering(
        const std::vector< std::vector<double> >& points,
        size_t chr_size, double eps, size_t min_pts);

#endif

#endif //EVOLUTION_OPTICS_H

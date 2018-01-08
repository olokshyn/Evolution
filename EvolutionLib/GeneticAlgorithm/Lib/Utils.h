//
// Created by oleg on 08.01.18.
//

#ifndef EVOLUTION_UTILS_H
#define EVOLUTION_UTILS_H

#include "GeneticAlgorithm/GAFwd.h"
#include "Entity/Entity.h"

double ScaleEpsToEntitiesNorms(const World* world,
                               LIST_TYPE(EntityPtr) new_entities,
                               double eps);

#endif //EVOLUTION_UTILS_H

//
// Created by Oleg on 9/18/16.
//

#ifndef EVOLUTION_TESTSCOMMON_H
#define EVOLUTION_TESTSCOMMON_H

extern "C" {
#include "Common.h"
#include "Entity/Entity.h"
#include "Species/Species.h"
}

extern const Objective random_objective;

Entity* MockCreateEntity(size_t chr_size,
                         const Objective* obj = &random_objective);

Species* MockCreateSpecies(size_t size, size_t chr_size,
                           const Objective* obj = &random_objective);

#endif //EVOLUTION_TESTSCOMMON_H

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

Entity* MockCreateEntity(size_t chr_size, Objective* obj = NULL);

Species* MockCreateSpecies(size_t size, size_t chr_size, Objective* obj = NULL);

#endif //EVOLUTION_TESTSCOMMON_H

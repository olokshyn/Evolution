//
// Created by Oleg on 9/13/16.
//

#ifndef EVOLUTION_CROSSOVERS_H
#define EVOLUTION_CROSSOVERS_H

#include <stdbool.h>

#include "Entity.h"
#include "GeneticAlgorithm/GAObjective.h"

bool OnePointCrossover(const Entity* parent1,
                       const Entity* parent2,
                       Entity* child1,
                       Entity* child2,
                       const Objective* obj,
                       double fitness1,  // normalized
                       double fitness2,  // normalized
                       size_t chromosome_size,
                       size_t generation_number,
                       size_t max_generations_count);

bool DHXCrossover(const Entity* parent1,
                  const Entity* parent2,
                  Entity* child1,
                  Entity* child2,
                  const Objective* obj,
                  double fitness1,  // normalized
                  double fitness2,  // normalized
                  size_t chromosome_size,
                  size_t generation_number,
                  size_t max_generations_count);

#endif //EVOLUTION_CROSSOVERS_H

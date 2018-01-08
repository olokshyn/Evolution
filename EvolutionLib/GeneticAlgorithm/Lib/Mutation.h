//
// Created by oleg on 08.01.18.
//

#ifndef EVOLUTION_MUTATION_H
#define EVOLUTION_MUTATION_H

#include <stddef.h>
#include <stdbool.h>

#include "GeneticAlgorithm/GAFwd.h"

double Mutation_NonUniformDelta(size_t t, double y,
                                const GAParameters* parameters);

bool Mutation_Uniform(World* world, size_t generation_number);

bool Mutation_NonUniform(World* world, size_t generation_number);

#endif //EVOLUTION_MUTATION_H

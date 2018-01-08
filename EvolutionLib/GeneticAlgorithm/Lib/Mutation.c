//
// Created by oleg on 08.01.18.
//

#include "Mutation.h"

#include "GeneticAlgorithm/World.h"
#include "GeneticAlgorithm/GAParameters.h"
#include "Logging/Logging.h"

double Mutation_NonUniformDelta(size_t t, double y,
                                const GAParameters* parameters) {
    double r = getRand(0.0, 1.0);
    return y * (1 - pow(r,
                        pow(1 - t / (double)parameters->max_generations_count,
                            parameters->mutation_on_iteration_dependence)));
}

bool Mutation_Uniform(World* world, size_t generation_number) {
    LOG_FUNC_START;

    if (!world->population) {
        goto error;
    }
    list_for_each(SpeciesPtr, world->population, species_var) {
        list_for_each(EntityPtr,
                      list_var_value(species_var)->entities,
                      entity_var) {
            if (doWithProbability(world->parameters->mutation_probability)) {
                size_t i = (size_t)selectRandom(0, (int)world->chr_size - 1);
                list_var_value(entity_var)->chr[i] =
                        getRand(world->parameters->objective->min,
                                world->parameters->objective->max);
                list_var_value(entity_var)->fitness =
                        world->parameters->objective->func(
                                list_var_value(entity_var)->chr,
                                world->chr_size);
            }
        }
    }
    LOG_FUNC_SUCCESS;
    return true;

error:
    LOG_FUNC_ERROR;
    return false;
}

bool Mutation_NonUniform(World* world, size_t generation_number) {
    LOG_FUNC_START;

    if (!world->population) {
        goto error;
    }
    list_for_each(SpeciesPtr, world->population, species_var) {
        list_for_each(EntityPtr,
                      list_var_value(species_var)->entities,
                      entity_var) {
            if (doWithProbability(world->parameters->mutation_probability)) {
                size_t i = (size_t)selectRandom(0, (int)world->chr_size - 1);
                if (doWithProbability(0.5)) {
                    list_var_value(entity_var)->chr[i] +=
                            Mutation_NonUniformDelta(
                                    generation_number,
                                    world->parameters->objective->max
                                    - list_var_value(entity_var)->chr[i],
                                    world->parameters);
                }
                else {
                    list_var_value(entity_var)->chr[i] -=
                            Mutation_NonUniformDelta(
                                    generation_number,
                                    list_var_value(entity_var)->chr[i]
                                    - world->parameters->objective->min,
                                    world->parameters);
                }
                list_var_value(entity_var)->fitness =
                        world->parameters->objective->func(list_var_value(entity_var)->chr,
                                                           world->chr_size);
            }
        }
    }
    LOG_FUNC_SUCCESS;
    return true;

error:
    LOG_FUNC_ERROR;
    return false;
}

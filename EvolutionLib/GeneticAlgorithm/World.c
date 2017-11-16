//
// Created by Oleg on 12/24/16.
//

#include "World.h"

#include <stdbool.h>

#include "GAParameters.h"
#include "GAOperators.h"

#include "DeathManager/DeathManager.h"
#include "Logging/Logging.h"
#include "Common.h"

static bool InitPopulation(World* world);

World* CreateWorld(const GAParameters* parameters,
                   const GAOperators* operators) {
    LOG_FUNC_START;

    if (!parameters) {
        Log(ERROR, "Cannot create world with NULL parameters");
        return NULL;
    }
    if (!operators) {
        Log(ERROR, "Cannot create world with NULL operators");
        return NULL;
    }

    if (!InitDeathManager("death.log")) {
        Log(ERROR, "Failed to init death manager");
        return NULL;
    }

    World* world = (World*)malloc(sizeof(World));
    if (!world) {
        goto error;
    }

    world->population = list_create(SpeciesPtr);
    if (!world->population) {
        goto destroy_world;
    }

    world->size = parameters->initial_world_size;
    world->chr_size = parameters->chromosome_size;
    if (parameters->objective.max_args_count > 0) {
        world->chr_size = MIN(parameters->chromosome_size,
                              parameters->objective.max_args_count);
    }

    world->parameters = (GAParameters*)malloc(sizeof(GAParameters));
    if (!world->parameters) {
        goto destroy_population;
    }
    *world->parameters = *parameters;

    world->operators = (GAOperators*)malloc(sizeof(GAOperators));
    if (!world->operators) {
        goto destroy_parameters;
    }
    *world->operators = *operators;

    if (!InitPopulation(world)) {
        goto destroy_operators;
    }

    LOG_FUNC_END;

    return world;

destroy_operators:
    free(world->operators);
destroy_parameters:
    free(world->parameters);
destroy_population:
    DestroyPopulation(world->population);
destroy_world:
    free(world);
error:
    ReleaseDeathManager();
    return NULL;
}

void DestroyWorld(World* world) {
    if (!world) {
        return;
    }
    free(world->operators);
    free(world->parameters);
    DestroyPopulation(world->population);
    free(world);
    ReleaseDeathManager();
}

static bool InitPopulation(World* world) {
    if (!world || !world->population) {
        return false;
    }

    Species* species = CreateSpecies(world->size);
    if (!species) {
        return false;
    }

    Entity* entity = NULL;
    for (size_t i = 0; i != world->size; ++i) {
        entity = CreateEntity(world->chr_size);
        if (!entity) {
            goto destroy_species;
        }

        for (size_t j = 0; j != world->chr_size; ++j) {
            entity->chr[j] =
                    getRand(world->parameters->objective.min,
                            world->parameters->objective.max);
        }
        entity->fitness =
                world->parameters->objective.func(entity->chr,
                                                  (int)world->chr_size);
        entity->old = true;

        if (!list_push_back(EntityPtr, species->entities, entity)) {
            goto destroy_entity;
        }
        entity = NULL;
    }
    if (!list_push_back(SpeciesPtr, world->population, species)) {
        goto destroy_species;
    }

    return true;

destroy_entity:
    DestroyEntity(entity);
destroy_species:
    DestroySpecies(species);
    return false;
}

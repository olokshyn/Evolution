//
// Created by Oleg on 12/24/16.
//

#include "World.h"

#include "GAParameters.h"
#include "GAOperators.h"
#include "DeathManager/DeathManager.h"
#include "Logging/Logging.h"
#include "Common.h"

World* CreateWorld(const GAParameters* parameters,
                   const GAOperators* operators) {
    LOG_FUNC_START("CreateWorld");

    if (!parameters) {
        Log(ERROR, "Cannot create world with NULL parameters");
        return NULL;
    }

    if (!InitDeathManager("death.log")) {
        Log(ERROR, "Failed to init death manager");
        return NULL;
    }

    World* world = NULL;
    Species* new_species = NULL;
    Entity* new_entity = NULL;

    world = (World*)malloc(sizeof(World));
    if (!world) {
        Log(ERROR, "Failed to allocate World object");
        goto error_CreateWorld;
    }

    InitSpeciesList(&world->species);
    world->world_size = parameters->initial_world_size;
    world->chr_size = parameters->chromosome_size;
    world->parameters = NULL;
    world->operators = NULL;

    if (parameters->objective.max_args_count > 0) {
        world->chr_size = MIN(parameters->chromosome_size,
                              parameters->objective.max_args_count);
    }

    GAParameters* params_copy = (GAParameters*)malloc(sizeof(GAParameters));
    if (!params_copy) {
        Log(ERROR, "Failed to allocate GAParameters");
        goto error_CreateWorld;
    }
    *params_copy = *parameters;
    world->parameters = params_copy;
    params_copy = NULL;


    GAOperators* ops_copy = (GAOperators*)malloc(sizeof(GAOperators));
    if (!ops_copy) {
        Log(ERROR, "Failed to allocate GAOperators");
        goto error_CreateWorld;
    }
    *ops_copy = *operators;
    world->operators = ops_copy;
    ops_copy = NULL;

    new_species = CreateSpecies(world->world_size);
    if (!new_species) {
        Log(ERROR, "Failed to allocate new species");
        goto error_CreateWorld;
    }

    for (size_t i = 0; i != world->world_size; ++i) {
        new_entity = CreateEntity(world->chr_size);
        if (!new_entity) {
            goto error_CreateWorld;
        }
        for (size_t j = 0; j != world->chr_size; ++j) {
            new_entity->chr[j] =
                    getRand(world->parameters->objective.min,
                            world->parameters->objective.max);
        }
        new_entity->fitness =
                world->parameters->objective.func(new_entity->chr,
                                                  (int)world->chr_size);
        new_entity->old = 1;
        if (!pushBack(new_species->entitiesList, new_entity)) {
            goto error_CreateWorld;
        }
        new_entity = NULL;
    }
    if (!pushBack(&world->species, new_species)) {
        goto error_CreateWorld;
    }
    new_species = NULL;

    LOG_FUNC_END("CreateWorld");

    return world;

    error_CreateWorld:
    ReleaseDeathManager();
    ClearWorld(&world);
    DestroySpecies(new_species);
    DestroyEntity(new_entity);
    return NULL;
}

void ClearWorld(World** world) {
    if (!world) {
        return;
    }
    if (*world) {
        destroyList(&(*world)->species);
        free((void*)(*world)->parameters);
        free((void*)(*world)->operators);
        free(*world);
        *world = NULL;
    }
    ReleaseDeathManager();
}
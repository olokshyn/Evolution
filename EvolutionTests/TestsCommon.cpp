//
// Created by Oleg on 9/18/16.
//

#include "TestsCommon.h"

Entity* MockCreateEntity(size_t chr_size, Objective* obj) {
    Entity* new_entity = CreateEntity(chr_size);
    if (!new_entity) {
        goto error_MockCreateEntity;
    }
    for (size_t j = 0; j < chr_size; ++j) {
        if (obj) {
            new_entity->chr[j] = getRand(obj->min, obj->max);
        }
        else {
            new_entity->chr[j] = getRand(0, 1);
        }
    }
    new_entity->fitness = obj ? obj->func(new_entity->chr, (int)chr_size) : 0.0;
    new_entity->old = 1;

    return new_entity;

error_MockCreateEntity:
    return NULL;
}

Species* MockCreateSpecies(size_t size, size_t chr_size, Objective* obj) {
    Species* new_species = NULL;
    Entity* new_entity = NULL;

    new_species = CreateSpecies(size);
    if (!new_species) {
        goto error_MockCreateSpecies;
    }
    for (size_t i = 0; i < size; ++i) {
        new_entity = MockCreateEntity(chr_size, obj);
        if (!new_entity) {
            goto error_MockCreateSpecies;
        }
        if (!pushBack(new_species->entitiesList, new_entity)) {
            goto error_MockCreateSpecies;
        }
        new_entity = NULL;
    }

    return new_species;

error_MockCreateSpecies:
    DestroyEntity(new_entity);
    DestroySpecies(new_species);
    return NULL;
}
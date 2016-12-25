//
// Created by Oleg on 5/22/16.
//

#include "Entity.h"

#include <math.h>

#include "Common.h"

int EntityComparator(const void* a, const void* b) {
    double diff = (*(Entity**)b)->fitness
                  - (*(Entity**)a)->fitness;
    return (fabs(diff) < DOUBLE_EPS) ? 0 : (diff > 0) - (diff < 0);
}

Entity* CreateEntity(size_t chr_size) {
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    if (!entity) {
        goto  error_CreateEntity;
    }
    entity->chr = (double*)malloc(sizeof(double) * chr_size);
    if (!entity->chr) {
        goto error_CreateEntity;
    }

    return entity;

error_CreateEntity:
    free(entity);
    return NULL;
}

void DestroyEntity(Entity* entity) {
    if (entity) {
        free(entity->chr);
        free(entity);
    }
}

Entity* CopyEntity(Entity* entity, size_t chr_size) {
    Entity* new_entity = (Entity*)malloc(sizeof(Entity));
    if (!new_entity) {
        return NULL;
    }
    new_entity->chr = (double*)malloc(sizeof(double) * chr_size);
    if (!new_entity->chr) {
        free(new_entity);
        return NULL;
    }
    for (size_t i = 0; i < chr_size; ++i) {
        new_entity->chr[i] = entity->chr[i];
    }
    new_entity->fitness = entity->fitness;
    new_entity->old = entity->old;
    return new_entity;
}

EntitiesList* CreateEntitiesList() {
    EntitiesList* entities = (EntitiesList*)malloc(sizeof(List));
    if (!entities) {
        return NULL;
    }
    initList(entities, EntityComparator, (void (*)(void*))DestroyEntity);
    return entities;
}

void MarkAllAsNew(EntitiesList* entities) {
    if (!entities || !entities->length) {
        return;
    }
    FOR_EACH_IN_ENTITIES(entities) {
        ENTITIES_IT_P->old = 0;
    }
}

void MarkAllAsOld(EntitiesList* entities) {
    if (!entities || !entities->length) {
        return;
    }
    FOR_EACH_IN_ENTITIES(entities) {
        ENTITIES_IT_P->old = 1;
    }
}

List* NormalizeEntitiesFitnesses(EntitiesList* entities) {
    List* fitness_list = NULL;
    double* fitness = NULL;

    fitness_list = (List*)malloc(sizeof(List));
    if (!fitness_list) {
        goto error_NormalizeEntitiesFitnesses;
    }
    initList(fitness_list, NULL, free);

    FOR_EACH_IN_ENTITIES(entities) {
        fitness = (double*)malloc(sizeof(double));
        if (!fitness) {
            goto error_NormalizeEntitiesFitnesses;
        }
        *fitness = ENTITIES_IT_P->fitness;
        if (!pushBack(fitness_list, fitness)) {
            goto error_NormalizeEntitiesFitnesses;
        }
        fitness = NULL;
    }

    Scale(fitness_list, 0.1, 0.9);

    return fitness_list;

error_NormalizeEntitiesFitnesses:
    destroyListPointer(fitness_list);
    free(fitness);
    return NULL;
}
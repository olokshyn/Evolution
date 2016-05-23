//
// Created by Oleg on 5/22/16.
//

#include <math.h>
#include "Entity.h"

#define DOUBLE_EPS 0.0001


int EntityComparator(const void* a, const void* b) {
    double diff = (*(Entity**)b)->fitness
                  - (*(Entity**)a)->fitness;
    return (fabs(diff) < DOUBLE_EPS) ? 0 : (diff > 0) - (diff < 0);
}

void EntityDestructor(Entity* entity) {
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
    return new_entity;
}

List* CreateEntitiesList() {
    List* entities = (List*)malloc(sizeof(List));
    if (!entities) {
        return NULL;
    }
    initList(entities, EntityComparator, (void (*)(void*))EntityDestructor);
    return entities;
}
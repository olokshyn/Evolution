//
// Created by Oleg on 5/22/16.
//

#ifndef EVOLUTION_ENTITY_H
#define EVOLUTION_ENTITY_H

#include "Common.h"
#include "List/List.h"

typedef struct entity {
    double* chr;
    double fitness;
    char old;
} Entity;

int EntityComparator(const void* a, const void* b);

Entity* CreateEntity(size_t chr_size);

void EntityDestructor(Entity* entity);

Entity* CopyEntity(Entity* entity, size_t chr_size);

List* CreateEntitiesList();

void MarkAllAsNew(List* entitiesList);

void MarkAllAsOld(List* entitiesList);

#endif //EVOLUTION_ENTITY_H

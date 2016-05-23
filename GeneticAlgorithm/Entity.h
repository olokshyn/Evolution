//
// Created by Oleg on 5/22/16.
//

#ifndef WISHART_ENTITY_H
#define WISHART_ENTITY_H

#include "../List/List.h"

typedef struct entity {
    double* chr;
    double fitness;
} Entity;

int EntityComparator(const void* a, const void* b);
void EntityDestructor(Entity* entity);
Entity* CopyEntity(Entity* entity, size_t chr_size);
List* CreateEntitiesList();

#endif //WISHART_ENTITY_H

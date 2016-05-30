//
// Created by Oleg on 5/14/16.
//

#include <math.h>

#include "Cluster.hpp"

size_t Cluster::vector_length = 0;

void Cluster::SetVectorLength(size_t vector_length) {
    Cluster::vector_length = vector_length;
}

Cluster::Cluster()
        : entities(CreateEntitiesList()) {
    if (!entities) {
        throw std::bad_alloc();
    }
}

Cluster::Cluster(List* entities)
        : entities(CreateEntitiesList()) {
    if (!entities) {
        throw std::bad_alloc();
    }
    copyList(this->entities, entities, entityCopier);
}

Cluster::Cluster(Entity* entity)
        : entities(CreateEntitiesList()) {
    if (!entities) {
        throw std::bad_alloc();
    }
    Entity* new_entity = CopyEntity(entity, vector_length);
    if (!new_entity) {
        throw std::bad_alloc();
    }
    pushBack(entities, new_entity);
}

Cluster::Cluster(const Cluster& other)
        : entities(CreateEntitiesList()) {
    if (!entities) {
        throw std::bad_alloc();
    }
    copyList(entities, other.entities, entityCopier);
}

Cluster::~Cluster() {
    clearListPointer(entities);
}

void Cluster::Add(Cluster& other) {
    List* other_entities = other.Release();
    moveList(entities, other_entities);
    clearListPointer(other_entities);
}

size_t Cluster::GetSize() const {
    return entities->length;
}

List* Cluster::Release() {
    List* temp = entities;
    entities = NULL;
    return temp;
}

double Cluster::GetNormSum(const Cluster& other) const {
    double sum = 0.0;
    for (ListIterator it1 = begin(entities);
            !isIteratorAtEnd(it1);
            next(&it1)) {
        for (ListIterator it2 = begin(other.entities);
                !isIteratorAtEnd(it2);
                next(&it2)) {
            sum += measure((Entity*)it1.current->value,
                           (Entity*)it2.current->value);
        }
    }
    return sum;
}

double Cluster::GetDistance(const Cluster& other) const {
    return GetNormSum(other)
            / (double)entities->length
            / (double)other.entities->length;
}

void* Cluster::entityCopier(void* value) {
    return CopyEntity((Entity*)value, vector_length);
}

double Cluster::measure(Entity* entity1, Entity* entity2) {
    double sum = 0.0;
    for (size_t i = 0; i < vector_length; ++i) {
        sum += pow(entity1->chr[i] - entity2->chr[i], 2);
    }
    return sqrt(sum);
}
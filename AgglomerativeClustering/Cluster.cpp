//
// Created by Oleg on 5/14/16.
//

#include <math.h>

#include "Cluster.hpp"

extern "C" {
#include "../DeathManager/DeathManager.h"
}

size_t Cluster::vector_length = 0;

void Cluster::SetVectorLength(size_t vector_length) {
    Cluster::vector_length = vector_length;
}

Cluster::Cluster()
        : species(CreateSpecies(0)) {
    if (!species) {
        throw std::bad_alloc();
    }
}

Cluster::Cluster(Species* species)
        : species(CreateSpecies(species->initial_size)) {
    if (!this->species) {
        throw std::bad_alloc();
    }
    this->species->died = species->died;
    copyList(this->species->entitiesList, species->entitiesList, entityCopier);
}

Cluster::Cluster(Entity* entity)
        : species(CreateSpecies(0)) {
    if (!species) {
        throw std::bad_alloc();
    }
    Entity* new_entity = (Entity*)entityCopier(entity);
    if (!new_entity) {
        throw std::bad_alloc();
    }
    pushBack(species->entitiesList, new_entity);
}

Cluster::Cluster(const Cluster& other)
        : species(CreateSpecies(other.species->initial_size)) {
    if (!species) {
        throw std::bad_alloc();
    }
    species->died = other.species->died;
    copyList(species->entitiesList, other.species->entitiesList, entityCopier);
}

Cluster::~Cluster() {
    ClearSpecies(species);
}

void Cluster::Add(Cluster& other) {
    Species* other_species = other.Release();
    if (other_species->initial_size != 0) {
        RegisterDeath();
    }
    MarkAllAsNew(other_species->entitiesList);
    moveList(species->entitiesList, other_species->entitiesList);
    ClearSpecies(other_species);
}

size_t Cluster::GetSize() const {
    return SPECIES_LENGTH(species);
}

Species* Cluster::Release() {
    Species* temp = species;
    species = NULL;
    return temp;
}

double Cluster::GetNormSum(const Cluster& other) const {
    double sum = 0.0;
    FOR_EACH_IN_SPECIES_N(species, it1) {
        FOR_EACH_IN_SPECIES_N(other.species, it2) {
            sum += measure(ENTITY_SP_IT_N(it1), ENTITY_SP_IT_N(it2));
        }
    }
    return sum;
}

double Cluster::GetDistance(const Cluster& other) const {
    return GetNormSum(other)
            / (double)SPECIES_LENGTH(species)
            / (double)SPECIES_LENGTH(other.species);
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
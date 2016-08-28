//
// Created by Oleg on 3/13/16.
//

#ifndef EVOLUTION_CLUSTER_H
#define EVOLUTION_CLUSTER_H

extern "C" {
#include "List/List.h"
#include "Species/Species.h"
};

#include <new>

class Cluster {

public:
    static void SetVectorLength(size_t vector_length);

    Cluster();
    explicit Cluster(Species* species);
    explicit Cluster(Entity* entity);
    explicit Cluster(const Cluster& other);

    ~Cluster();

    void Add(Cluster& other);
    size_t GetSize() const;
    Species* Release();

    double GetNormSum(const Cluster& other) const;
    double GetDistance(const Cluster& other) const;

private:
    Species* species;

    static size_t vector_length;

    static void* entityCopier(void* value);
    static double measure(Entity* entity1, Entity* entity2);
};

#endif //EVOLUTION_CLUSTER_H

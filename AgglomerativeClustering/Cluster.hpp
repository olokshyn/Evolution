//
// Created by Oleg on 3/13/16.
//

#ifndef WISHART_CLUSTER_H
#define WISHART_CLUSTER_H

extern "C" {
#include "../List/List.h"
#include "../GeneticAlgorithm/Entity.h"
#include "../GeneticAlgorithm/Species.h"
};

#include <new>

class Cluster {

public:
    static void SetVectorLength(size_t vector_length);

    Cluster();
    Cluster(Species* species);
    Cluster(Entity* entity);
    Cluster(const Cluster& other);

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

#endif //WISHART_CLUSTER_H

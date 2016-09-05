//
// Created by Oleg on 8/28/16.
//

#include <vector>
#include <utility>
#include <stdexcept>

#include "gtest/gtest.h"

extern "C" {
#include <math.h>

#include "List/List.h"
#include "Entity/Entity.h"
#include "Species/Species.h"
}

#include "AgglomerativeClustering/Cluster.hpp"

using namespace std;

namespace {
    const size_t chr_size = 10;

    Entity* MockCreateEntity(size_t chr_size) {
        Entity* new_entity = CreateEntity(chr_size);
        if (!new_entity) {
            goto error_MockCreateEntity;
        }
        for (int j = 0; j < chr_size; ++j) {
            new_entity->chr[j] = getRand(0, 1);
        }
        new_entity->fitness = 0.0;
        new_entity->old = 1;

        return new_entity;

    error_MockCreateEntity:
        throw runtime_error("MockCreateEntity");
    }

    Species* MockCreateSpecies(size_t size, size_t chr_size) {
        Species* new_species = NULL;
        Entity* new_entity = NULL;

        new_species = CreateSpecies(size);
        if (!new_species) {
            goto error_MockCreateSpecies;
        }
        for (size_t i = 0; i < size; ++i) {
            new_entity = CreateEntity(chr_size);
            if (!new_entity) {
                goto error_MockCreateSpecies;
            }
            for (int j = 0; j < chr_size; ++j) {
                new_entity->chr[j] = getRand(0, 1);
            }
            new_entity->fitness = 0.0;
            new_entity->old = 1;
            if (!pushBack(new_species->entitiesList, new_entity)) {
                goto error_MockCreateSpecies;
            }
            new_entity = NULL;
        }

        return new_species;

    error_MockCreateSpecies:
        EntityDestructor(new_entity);
        ClearSpecies(new_species);
        throw runtime_error("MockCreateSpecies");
    }

    double EuclidMeasure(double* x, double* y, size_t size) {
        double sum = 0.0;
        for (size_t i = 0; i < size; ++i) {
            sum += pow(x[i] - y[i], 2);
        }
        return sqrt(sum);
    }
}

TEST(ClusterTest, Creation) {
    Cluster::SetVectorLength(chr_size);

    Species* species = MockCreateSpecies(10, chr_size);
    ASSERT_NO_THROW(Cluster cluster(species));
    ClearSpecies(species);

    Entity* entity = MockCreateEntity(chr_size);
    ASSERT_NO_THROW(Cluster(entity));
    EntityDestructor(entity);

    ASSERT_NO_THROW(Cluster(cluster));
}

TEST(ClusterTest, CopyConstructor) {
    Cluster::SetVectorLength(chr_size);

    Entity* entity = MockCreateEntity(chr_size);
    Cluster cluster(entity);
    EntityDestructor(entity);

    Cluster cluster2(cluster);

    Species* sp1 = cluster.Release();
    Species* sp2 = cluster2.Release();

    ASSERT_NE(sp1, sp2);
    ASSERT_NE(sp1->entitiesList->head->value, sp2->entitiesList->head->value);

    ClearSpecies(sp1);
    ClearSpecies(sp2);
}

TEST(ClusterTest, Add) {
    Cluster::SetVectorLength(chr_size);

    Cluster cluster;

    {
        Entity* entity = MockCreateEntity(chr_size);
        Cluster cluster2(entity);
        EntityDestructor(entity);
        cluster.Add(cluster2);
        ASSERT_EQ(0, cluster2.GetSize());
    }

    ASSERT_EQ(1, cluster.GetSize());

    ASSERT_NO_THROW(Cluster(cluster));
}

TEST(ClusterTest, Vector) {
    Cluster::SetVectorLength(chr_size);

    vector<Cluster> vec;

    Entity* entity;
    Species* species;

    {
        vector<Cluster> vec2;

        vec2.push_back(Cluster(entity = MockCreateEntity(chr_size)));
        EntityDestructor(entity);

        vec2.push_back(Cluster(species = MockCreateSpecies(10, chr_size)));
        ClearSpecies(species);

        vec = move(vec2);
        ASSERT_EQ(0, vec2.size());
    }

    ASSERT_EQ(1, vec[0].GetSize());
    ASSERT_EQ(10, vec[1].GetSize());
}

TEST(ClusterTest, GetNormSum) {
    Species* sp1 = MockCreateSpecies(15, chr_size);
    Species* sp2 = MockCreateSpecies(20, chr_size);

    Cluster cl1(sp1);
    Cluster cl2(sp2);

    double norm = 0.0;
    FOR_EACH_IN_SPECIES_N(sp1, it1) {
        FOR_EACH_IN_SPECIES_N(sp2, it2) {
            norm += EuclidMeasure(ENTITY_SP_IT_N(it1)->chr,
                                  ENTITY_SP_IT_N(it2)->chr,
                                  chr_size);
        }
    }

    ClearSpecies(sp1);
    ClearSpecies(sp2);

    ASSERT_EQ(norm, cl1.GetNormSum(cl2));
}
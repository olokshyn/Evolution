//
// Created by Oleg on 8/28/16.
//

#include <vector>

#include "gtest/gtest.h"

extern "C" {
#include "Common.h"
#include "Entity/Entity.h"
#include "Species/Species.h"
}

#include "AgglomerativeClustering/Cluster.hpp"

#include "TestsCommon.h"

using namespace std;

namespace {
    const size_t chr_size = 10;
}

TEST(ClusterTest, Creation) {
    Cluster::SetVectorLength(chr_size);

    Species* species = MockCreateSpecies(10, chr_size);
    ASSERT_NE((void*)0, species);
    ASSERT_NO_THROW(Cluster cluster(species));
    ClearSpecies(species);

    Entity* entity = MockCreateEntity(chr_size);
    ASSERT_NE((void*)0, entity);
    ASSERT_NO_THROW(Cluster(entity));
    EntityDestructor(entity);

    ASSERT_NO_THROW(Cluster(cluster));
}

TEST(ClusterTest, CopyConstructor) {
    Cluster::SetVectorLength(chr_size);

    Entity* entity = MockCreateEntity(chr_size);
    ASSERT_NE((void*)0, entity);
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
        ASSERT_NE((void*)0, entity);
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

        entity = MockCreateEntity(chr_size);
        ASSERT_NE((void*)0, entity);
        vec2.push_back(Cluster(entity));
        EntityDestructor(entity);

        species = MockCreateSpecies(10, chr_size);
        ASSERT_NE((void*)0, species);
        vec2.push_back(Cluster(species));
        ClearSpecies(species);

        vec = move(vec2);
        ASSERT_EQ(0, vec2.size());
    }

    ASSERT_EQ(1, vec[0].GetSize());
    ASSERT_EQ(10, vec[1].GetSize());
}

TEST(ClusterTest, GetNormSum) {
    Species* sp1 = MockCreateSpecies(15, chr_size);
    ASSERT_NE((void*)0, sp1);
    Species* sp2 = MockCreateSpecies(20, chr_size);
    ASSERT_NE((void*)0, sp2);

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
//
// Created by Oleg on 8/28/16.
//

#include <vector>

#include "gtest/gtest.h"

extern "C"
{
#include "Common.h"
#include "Entity/Entity.h"
#include "Species/Species.h"
}

#include "AgglomerativeClustering/Cluster.hpp"

#include "TestsCommon.h"

using namespace std;

namespace
{
    const size_t chr_size = 10;
}

TEST(ClusterTest, Creation)
{
    Cluster::set_chr_size(chr_size);

    Species* species = MockCreateSpecies(10, chr_size);
    ASSERT_NE(nullptr, species);
    Cluster cluster(species);
    DestroySpecies(species);

    Entity* entity = MockCreateEntity(chr_size);
    ASSERT_NE(nullptr, entity);
    ASSERT_NO_THROW(Cluster(entity));
    DestroyEntity(entity);

    ASSERT_NO_THROW(Cluster(std::move(cluster)));
}

TEST(ClusterTest, CopyConstructor)
{
    Cluster::set_chr_size(chr_size);

    Entity* entity = MockCreateEntity(chr_size);
    ASSERT_NE(nullptr, entity);
    Cluster cluster(entity);
    DestroyEntity(entity);

    Cluster cluster2(std::move(cluster));

    Species* sp = cluster2.release();
    ASSERT_NE(nullptr, sp);

    DestroySpecies(sp);
}

TEST(ClusterTest, Add)
{
    Cluster::set_chr_size(chr_size);

    Cluster cluster;

    {
        Entity* entity = MockCreateEntity(chr_size);
        ASSERT_NE(nullptr, entity);
        Cluster cluster2(entity);
        DestroyEntity(entity);
        cluster.add(cluster2);
        ASSERT_EQ(static_cast<size_t>(0), cluster2.size());
    }

    ASSERT_EQ(static_cast<size_t>(1), cluster.size());

    ASSERT_NO_THROW(Cluster(std::move(cluster)));
}

TEST(ClusterTest, Vector)
{
    Cluster::set_chr_size(chr_size);

    vector<Cluster> vec;

    {
        vector<Cluster> vec2;

        Entity* entity = MockCreateEntity(chr_size);
        ASSERT_NE(nullptr, entity);
        vec2.emplace_back(entity);
        DestroyEntity(entity);

        Species* species = MockCreateSpecies(10, chr_size);
        ASSERT_NE(nullptr, species);
        vec2.emplace_back(species);
        DestroySpecies(species);

        vec = std::move(vec2);
        ASSERT_EQ(static_cast<size_t>(0), vec2.size());
    }

    ASSERT_EQ(static_cast<size_t>(1), vec[0].size());
    ASSERT_EQ(static_cast<size_t>(10), vec[1].size());
}

TEST(ClusterTest, NormSum)
{
    Species* sp1 = MockCreateSpecies(15, chr_size);
    ASSERT_NE(nullptr, sp1);
    Species* sp2 = MockCreateSpecies(20, chr_size);
    ASSERT_NE(nullptr, sp2);

    Cluster cl1(sp1);
    Cluster cl2(sp2);

    double norm = 0.0;
    list_for_each(EntityPtr, sp1->entities, var1)
    {
        list_for_each(EntityPtr, sp2->entities, var2)
        {
            norm += EuclidMeasure(list_var_value(var1)->chr,
                                  list_var_value(var2)->chr,
                                  chr_size);
        }
    }

    DestroySpecies(sp1);
    DestroySpecies(sp2);

    ASSERT_EQ(norm, cl1.norm_sum(cl2));
}
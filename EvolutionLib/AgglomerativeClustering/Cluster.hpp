//
// Created by Oleg on 3/13/16.
//

#ifndef EVOLUTION_CLUSTER_H
#define EVOLUTION_CLUSTER_H

extern "C"
{
#include "GeneticAlgorithm/GAFwd.h"
#include "Species/Species.h"
};

#include <memory>

class Cluster
{
public:
    static void set_chr_size(size_t chr_size);
    static void set_journal(Journal* journal);

public:
    Cluster();
    explicit Cluster(const Species* species);
    explicit Cluster(const Entity* entity);

    Cluster(Cluster&& other) noexcept;

    void add(Cluster& other);
    Species* release();
    size_t size() const;

    double norm_sum(const Cluster& other) const;
    double distance(const Cluster& other) const;

private:
    static std::unique_ptr<Entity, decltype(&DestroyEntity)> copy_entity(
            const Entity* entity);
    static double measure(const Entity* entity1, const Entity* entity2);

private:
    static thread_local size_t s_chr_size;
    static thread_local Journal* s_journal;

private:
    std::unique_ptr<Species, decltype(&DestroySpecies)> m_species;
};

#endif //EVOLUTION_CLUSTER_H

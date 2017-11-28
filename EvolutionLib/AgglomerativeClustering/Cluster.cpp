//
// Created by Oleg on 5/14/16.
//

#include "Cluster.hpp"

#include <cmath>

extern "C"
{
#include "Logging/Logging.h"
#include "Journal/Journal.h"
}

thread_local size_t Cluster::s_chr_size = 0;
thread_local Journal* Cluster::s_journal = nullptr;

void Cluster::set_chr_size(size_t chr_size)
{
    Cluster::s_chr_size = chr_size;
}

void Cluster::set_journal(Journal* journal)
{
    Cluster::s_journal = journal;
}

Cluster::Cluster()
        : m_species(CreateSpecies(0), &DestroySpecies)
{
    if (!m_species)
    {
        throw std::bad_alloc();
    }
}

Cluster::Cluster(const Species* species)
        : m_species(CreateSpecies(species->initial_size), &DestroySpecies)
{
    if (!m_species)
    {
        throw std::bad_alloc();
    }
    m_species->died = species->died;
    list_for_each(EntityPtr, species->entities, var)
    {
        auto copy = copy_entity(list_var_value(var));
        if (!list_push_back(EntityPtr, m_species->entities, copy.get()))
        {
            throw std::bad_alloc();
        }
        copy.release();
    }
}

Cluster::Cluster(const Entity* entity)
        : m_species(CreateSpecies(0), &DestroySpecies)
{
    if (!m_species)
    {
        throw std::bad_alloc();
    }
    auto copy = copy_entity(entity);
    if (!list_push_back(EntityPtr, m_species->entities, copy.get()))
    {
        throw std::bad_alloc();
    }
    copy.release();
}

Cluster::Cluster(Cluster&& other) noexcept
    : m_species(std::move(other.m_species))
{
    LOG_ASSERT(m_species);
}

void Cluster::add(Cluster& other)
{
    LOG_ASSERT(m_species);
    LOG_ASSERT(other.m_species);
    if (other.m_species->initial_size != 0)
    {
        RecordSpeciesDeath(s_journal, other.m_species->initial_size);
    }
    SetEntitiesStatus(other.m_species->entities, false);
    if (!list_move(EntityPtr, other.m_species->entities, m_species->entities))
    {
        throw std::runtime_error("Failed to move list");
    }
}

Species* Cluster::release()
{
    LOG_ASSERT(m_species);
    return m_species.release();
}

size_t Cluster::size() const
{
    LOG_ASSERT(m_species);
    return list_len(m_species->entities);
}

double Cluster::norm_sum(const Cluster& other) const
{
    LOG_ASSERT(m_species);
    LOG_ASSERT(other.m_species);
    double sum = 0.0;
    list_for_each(EntityPtr, m_species->entities, var1)
    {
        list_for_each(EntityPtr, other.m_species->entities, var2)
        {
            if (!list_var_eq(var1, var2))
            {
                sum += measure(list_var_value(var1), list_var_value(var2));
            }
        }
    }
    return sum;
}

double Cluster::distance(const Cluster& other) const
{
    return norm_sum(other) / static_cast<double>(size()) / other.size();
}

std::unique_ptr<Entity, decltype(&DestroyEntity)>
Cluster::copy_entity(const Entity* entity)
{
    std::unique_ptr<Entity, decltype(&DestroyEntity)>
            entity_copy(CopyEntity(entity, s_chr_size), &DestroyEntity);
    if (!entity_copy)
    {
        throw std::bad_alloc();
    }
    return entity_copy;
};

double Cluster::measure(const Entity* entity1, const Entity* entity2)
{
    double sum = 0.0;
    for (size_t i = 0; i != s_chr_size; ++i)
    {
        sum += pow(entity1->chr[i] - entity2->chr[i], 2.0);
    }
    return sqrt(sum);
}
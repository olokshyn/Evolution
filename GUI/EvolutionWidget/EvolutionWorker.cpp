//
// Created by Oleg on 11/21/17.
//

#include "EvolutionWidget/EvolutionWorker.h"

#include <utility>

extern "C"
{
#include "GeneticAlgorithm/GeneticAlgorithm.h"
}

#include "Utils/Utils.h"

namespace
{
    inline EvolutionWorker* worker(void* data)
    {
        return reinterpret_cast<EvolutionWorker*>(data);
    }
}

EvolutionWorker::EvolutionWorker(const GAParameters& parameters,
                                 const GAOperators& operators,
                                 std::string name,
                                 LogLevel log_level,
                                 QObject* parent)
        : QObject(parent),
          m_parameters(parameters),
          m_operators(operators),
          m_name(std::move(name)),
          m_log_level(log_level),
          m_world_size(0),
          m_stop_evolution(false)
{
    m_journal.data = this;
    m_journal.evolution_stop_requested = &EvolutionWorker::evolution_stop_requested;
    m_journal.iteration_start = &EvolutionWorker::iteration_start;
    m_journal.iteration_start = &EvolutionWorker::iteration_start;
    m_journal.iteration_end = &EvolutionWorker::iteration_end;
    m_journal.crossover = &EvolutionWorker::crossover;
    m_journal.children_selection = &EvolutionWorker::children_selection;
    m_journal.clustering = &EvolutionWorker::clustering;
    m_journal.before_selection = &EvolutionWorker::before_selection;
    m_journal.selection = &EvolutionWorker::selection;
    m_journal.species_death = &EvolutionWorker::species_death;
}

void EvolutionWorker::start_evolution()
{
    utils::LoggingGuard::singleton(m_name + ".log", m_log_level);

    GAResult result = RunEvolution(&m_parameters,
                                   &m_operators,
                                   &m_journal);
    if (result.error)
    {
        throw std::runtime_error("Failed to run evolution");
    }
    emit optimum_reached(result.optimum);
}

void EvolutionWorker::stop_evolution()
{
    m_stop_evolution = true;
}

bool EvolutionWorker::evolution_stop_requested(void* data)
{
    return worker(data)->evolution_stop_requested();
}

void EvolutionWorker::iteration_start(void* data,
                                      LIST_TYPE(SpeciesPtr) population,
                                      size_t generation_number)
{
    worker(data)->iteration_start(population, generation_number);
}

void EvolutionWorker::iteration_end(void* data,
                                    LIST_TYPE(SpeciesPtr) population,
                                    size_t species_died_on_iteration)
{
    worker(data)->iteration_end(population, species_died_on_iteration);
}

void EvolutionWorker::crossover(void* data,
                                LIST_TYPE(SpeciesPtr) population,
                                LIST_TYPE(EntityPtr) new_entities)
{
    worker(data)->crossover(population, new_entities);
}

void EvolutionWorker::children_selection(void* data,
                                         LIST_TYPE(SpeciesPtr) population,
                                         LIST_TYPE(EntityPtr) new_entities)
{
    worker(data)->children_selection(population, new_entities);
}

void EvolutionWorker::clustering(void* data,
                                 LIST_TYPE(SpeciesPtr) population,
                                 LIST_TYPE(SpeciesPtr) new_population)
{
    worker(data)->clustering(population, new_population);
}

void EvolutionWorker::before_selection(void* data,
                                       LIST_TYPE(SpeciesPtr) population,
                                       size_t world_size)
{
    worker(data)->before_selection(population, world_size);
}

void EvolutionWorker::selection(void* data,
                                LIST_TYPE(SpeciesPtr) population,
                                size_t world_size)
{
    worker(data)->selection(population, world_size);
}

void EvolutionWorker::species_death(void* data,
                                    size_t initial_size)
{
    worker(data)->species_death(initial_size);
}

bool EvolutionWorker::evolution_stop_requested() const
{
    return m_stop_evolution;
}

void EvolutionWorker::iteration_start(
        LIST_TYPE(SpeciesPtr) population,
        size_t generation_number)
{
    m_info = IterationInfo();
    m_world_size = 0;
    m_info.generation_number = generation_number;
}

void EvolutionWorker::iteration_end(
        LIST_TYPE(SpeciesPtr) population,
        size_t species_died_on_iteration)
{
    m_info.world_size = m_world_size;

    m_info.species.reserve(list_len(population));
    list_for_each(SpeciesPtr, population, var)
    {
        m_info.species.push_back(list_len(list_var_value(var)->entities));
    }

    m_info.fitnesses.reserve(m_world_size);
    list_for_each(SpeciesPtr, population, sp_var)
    {
        list_for_each(EntityPtr, list_var_value(sp_var)->entities, en_var)
        {
            m_info.fitnesses.push_back(list_var_value(en_var)->fitness);
        }
    }

    m_info.species_died = species_died_on_iteration;

    emit iteration_completed(m_info);
}

void EvolutionWorker::crossover(
        LIST_TYPE(SpeciesPtr) population,
        LIST_TYPE(EntityPtr) new_entities)
{
    m_info.new_entities_count = list_len(new_entities);
}

void EvolutionWorker::children_selection(
        LIST_TYPE(SpeciesPtr) population,
        LIST_TYPE(EntityPtr) new_entities)
{
    m_info.entities_died += m_info.new_entities_count - list_len(new_entities);
}

void EvolutionWorker::clustering(
        LIST_TYPE(SpeciesPtr) population,
        LIST_TYPE(SpeciesPtr) new_population)
{

}

void EvolutionWorker::before_selection(
        LIST_TYPE(SpeciesPtr) population,
        size_t world_size)
{
    m_world_size = world_size;
}

void EvolutionWorker::selection(
        LIST_TYPE(SpeciesPtr) population,
        size_t world_size)
{
    LOG_RELEASE_ASSERT(m_world_size >= world_size);
    m_info.entities_died += m_world_size - world_size;
    m_world_size = world_size;
}

void EvolutionWorker::species_death(
        size_t initial_size)
{

}

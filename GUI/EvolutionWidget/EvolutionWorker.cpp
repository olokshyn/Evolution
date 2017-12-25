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
                                 const GAOperators* operators,
                                 size_t buffer_size,
                                 std::string name,
                                 LogLevel log_level,
                                 QObject* parent)
        : QObject(parent),
          m_parameters(parameters),
          m_operators(operators),
          m_name(std::move(name)),
          m_log_level(log_level),
          m_world_size(0),
          m_buffer_size(buffer_size),
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

    m_stop_evolution = false;
    GAResult result = RunEvolution(&m_parameters,
                                   m_operators,
                                   &m_journal);
    flush_buffer();
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
                                    size_t chromosome_size,
                                    size_t species_died_on_iteration)
{
    worker(data)->iteration_end(population, chromosome_size, species_died_on_iteration);
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
        size_t chromosome_size,
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
            LOG_ASSERT(
                    DOUBLE_EQ(list_var_value(en_var)->fitness,
                              m_parameters.objective->func(list_var_value(en_var)->chr,
                                                           get_chromosome_size(&m_parameters))));
            m_info.fitnesses.push_back(list_var_value(en_var)->fitness);
        }
    }

    m_info.norms.reserve(m_world_size);
    const std::vector<double> zero(chromosome_size);
#ifndef NDEBUG
    for (auto value : zero)
    {
        LOG_ASSERT(value == 0.0);
    }
#endif
    m_info.norms.reserve(list_len(population));
    list_for_each(SpeciesPtr, population, sp_var)
    {
        std::vector<double> species_norms;
        species_norms.reserve(list_len(list_var_value(sp_var)->entities));
        list_for_each(EntityPtr, list_var_value(sp_var)->entities, en_var)
        {
            species_norms.push_back(
                    EuclidMeasure(list_var_value(en_var)->chr,
                                  zero.data(),
                                  chromosome_size));
        }
        m_info.norms.push_back(std::move(species_norms));
    }

    m_info.species_died = species_died_on_iteration;

    send_iteration(m_info);
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

void EvolutionWorker::send_iteration(const IterationInfo& info)
{
    m_buffered_infos.push_back(info);
    if (static_cast<size_t>(m_buffered_infos.size()) >= m_buffer_size)
    {
        emit iterations_done(m_buffered_infos);
        m_buffered_infos.clear();
    }
}

void EvolutionWorker::flush_buffer()
{
    if (!m_buffered_infos.empty())
    {
        emit iterations_done(m_buffered_infos);
        m_buffered_infos.clear();
    }
}

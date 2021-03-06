//
// Created by Oleg on 11/21/17.
//

#ifndef GUI_EVOLUTIONMANAGER_H
#define GUI_EVOLUTIONMANAGER_H

#include <memory>
#include <string>
#include <mutex>
#include <condition_variable>

#include <QObject>
#include <QList>

extern "C"
{
#include "GeneticAlgorithm/GAParameters.h"
#include "GeneticAlgorithm/GAOperators.h"
#include "Journal/Journal.h"
}

#include "IterationInfo.h"
#include "Utils/Utils.h"

class EvolutionWorker : public QObject
{
Q_OBJECT

public:
    EvolutionWorker(const GAParameters& parameters,
                    const GAOperators* operators,
                    size_t buffer_size,
                    std::string name,
                    LogLevel log_level = DEBUG,
                    QObject* parent = nullptr);

    void start_evolution();
    void stop_evolution();
    void pause_evolution();
    void resume_evolution();

signals:
    void iterations_done(const QList<IterationInfo>& infos);
    void optimum_reached(double optimum);

private:
    static bool evolution_stop_requested(void* data);
    static void iteration_start(void* data,
                                LIST_TYPE(SpeciesPtr) population,
                                size_t generation_number);
    static void iteration_end(void* data,
                              LIST_TYPE(SpeciesPtr) population,
                              size_t chromosome_size,
                              size_t species_died_on_iteration);
    static void crossover(void* data,
                          LIST_TYPE(SpeciesPtr) population,
                          LIST_TYPE(EntityPtr) new_entities);
    static void children_selection(void* data,
                                   LIST_TYPE(SpeciesPtr) population,
                                   LIST_TYPE(EntityPtr) new_entities);
    static void clustering(void* data,
                           LIST_TYPE(SpeciesPtr) population,
                           LIST_TYPE(SpeciesPtr) new_population);
    static void before_selection(void* data,
                                 LIST_TYPE(SpeciesPtr) population,
                                 size_t world_size);
    static void selection(void* data,
                          LIST_TYPE(SpeciesPtr) population,
                          size_t world_size);
    static void species_death(void* data,
                              size_t initial_size);

private:
    bool evolution_stop_requested() const;
    void iteration_start(
            LIST_TYPE(SpeciesPtr) population,
            size_t generation_number);
    void iteration_end(
            LIST_TYPE(SpeciesPtr) population,
            size_t chromosome_size,
            size_t species_died_on_iteration);
    void crossover(
            LIST_TYPE(SpeciesPtr) population,
            LIST_TYPE(EntityPtr) new_entities);
    void children_selection(
            LIST_TYPE(SpeciesPtr) population,
            LIST_TYPE(EntityPtr) new_entities);
    void clustering(
            LIST_TYPE(SpeciesPtr) population,
            LIST_TYPE(SpeciesPtr) new_population);
    void before_selection(
            LIST_TYPE(SpeciesPtr) population,
            size_t world_size);
    void selection(
            LIST_TYPE(SpeciesPtr) population,
            size_t world_size);
    void species_death(
            size_t initial_size);

    void send_iteration(const IterationInfo& info);
    void flush_buffer();

private:
    const GAParameters m_parameters;
    const GAOperators* m_operators;
    Journal m_journal;

    const std::string m_name;
    LogLevel m_log_level;

    IterationInfo m_info;
    size_t m_world_size;

    QList<IterationInfo> m_buffered_infos;
    size_t m_buffer_size;

    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cond_var;
    bool m_stop_evolution;
    bool m_pause_evolution;
};

#endif //GUI_EVOLUTIONMANAGER_H

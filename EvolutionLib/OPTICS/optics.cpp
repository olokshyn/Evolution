//
// Created by Oleg on 10/5/17.
//

#include <vector>
#include <list>
#include <memory>
#include <type_traits>
#include <iostream>
#include <stdexcept>

#include "optics.h"

#include "OpticsObjectsManager.h"
#include "OrderedSeeds.h"


extern "C"
{

#include <GeneticAlgorithm/World.h>
#include "Entity/Entity.h"
#include "Species/Species.h"

}

namespace
{

    std::unique_ptr<Entity, decltype(&DestroyEntity)> copy_entity(
            const Entity* entity, size_t chr_size)
    {
        std::unique_ptr<Entity, decltype(&DestroyEntity)>
                ecopy(CreateEntity(chr_size), &DestroyEntity);
        if (!ecopy)
        {
            throw std::bad_alloc();
        }
        for (size_t i = 0; i != chr_size; ++i)
        {
            ecopy->chr[i] = entity->chr[i];
        }
        ecopy->fitness = entity->fitness;
        ecopy->old = entity->old;
        return ecopy;
    };

    std::vector<OpticsObject> extract_optics_objects(
            LIST_TYPE(SpeciesPtr) population,
            LIST_TYPE(EntityPtr) new_entities)
    {
        size_t entities_count = new_entities
                                ? list_len(new_entities) : 0;
        list_for_each(SpeciesPtr, population, var)
        {
            entities_count += list_len(list_var_value(var)->entities);
        }

        std::vector<OpticsObject> objects;
        objects.reserve(entities_count);

        size_t index = 0;
        if (new_entities)
        {
            list_for_each(EntityPtr, new_entities, var)
            {
                objects.emplace_back(index++, list_var_value(var));
            }
        }
        list_for_each(SpeciesPtr, population, species_var)
        {
            list_for_each(EntityPtr,
                          list_var_value(species_var)->entities,
                          entity_var)
            {
                objects.emplace_back(index++, list_var_value(entity_var));
            }
        }
        return objects;
    }

    std::vector<OpticsObject> extract_optics_objects(
            const std::vector< std::vector<double> >& points,
            std::vector< std::unique_ptr<Entity, decltype(&DestroyEntity)> >& entities)
    {
        std::vector<OpticsObject> objects;
        objects.reserve(points.size());
        for (size_t i = 0; i != points.size(); ++i)
        {
            std::unique_ptr<Entity, decltype(&DestroyEntity)>
                    entity(CreateEntity(points[i].size()), &DestroyEntity);
            if (!entity)
            {
                throw std::bad_alloc();
            }
            for (size_t chr_i = 0; chr_i != points[i].size(); ++chr_i)
            {
                entity->chr[chr_i] = points[i][chr_i];
            }
            entity->fitness = 0.0;
            entity->old = false;
            objects.emplace_back(i, entity.get());
            entities.emplace_back(std::move(entity));
        }
        return objects;
    }

    void expand_cluster_order(OpticsObjectsManager& objects_manager,
                              OpticsObject& obj, double eps, size_t min_pts,
                              std::vector<OpticsObject>& cluster_ordering)
    {
        std::vector<size_t> neighbor_indexes =
                objects_manager.find_neighbor_indexes(obj, eps);
        obj.processed = true;
        objects_manager.set_core_distance(obj, neighbor_indexes, eps, min_pts);
        cluster_ordering.push_back(obj);

        if (is_distance_valid(obj.core_distance))
        {
            OrderedSeeds ordered_seeds = OrderedSeeds();
            ordered_seeds.update(objects_manager, neighbor_indexes, obj);
            while (!ordered_seeds.empty())
            {
                OpticsObject& current_obj =
                        objects_manager.object(ordered_seeds.pop_next());
                neighbor_indexes =
                        objects_manager.find_neighbor_indexes(current_obj, eps);
                current_obj.processed = true;
                objects_manager.set_core_distance(current_obj,
                                                  neighbor_indexes,
                                                  eps, min_pts);
                cluster_ordering.push_back(current_obj);
                if (is_distance_valid(current_obj.core_distance))
                {
                    ordered_seeds.update(objects_manager,
                                         neighbor_indexes,
                                         current_obj);
                }
            }
        }
    }

    std::vector<OpticsObject> optics(std::vector<OpticsObject>&& objects,
                                     size_t chr_size, double eps, size_t min_pts)
    {
        std::vector<OpticsObject> cluster_ordering;
        cluster_ordering.reserve(objects.size());

        OpticsObjectsManager objects_manager(std::move(objects), chr_size);
        for (size_t object_index = 0;
             object_index != objects_manager.size();
             ++object_index)
        {
            OpticsObject& obj = objects_manager.object(object_index);
            if (!obj.processed)
            {
                expand_cluster_order(objects_manager, obj,
                                     eps, min_pts, cluster_ordering);
            }
        }

        return cluster_ordering;
    }

    LIST_TYPE(SpeciesPtr) extract_clusters(
            size_t chr_size,
            const std::vector<OpticsObject>& cluster_ordering, double eps,
            std::vector<size_t>* cluster_labels = nullptr,
            LIST_TYPE(EntityPtr) noise_entities = nullptr)
    {
        std::unique_ptr<
                std::remove_pointer<LIST_TYPE(SpeciesPtr)>::type,
                decltype(&DestroyPopulation)>
                clusters(list_create(SpeciesPtr), &DestroyPopulation);
        if (!clusters)
        {
            throw std::bad_alloc();
        }

        std::unique_ptr<Species, decltype(&DestroySpecies)>
                current_cluster(nullptr, &DestroySpecies);
        size_t current_cluster_id = 0;

        for (auto& obj : cluster_ordering)
        {
            if (!is_distance_valid(obj.reachability_distance)
                || obj.reachability_distance > eps)
            {
                if (is_distance_valid(obj.core_distance)
                    && obj.core_distance <= eps)
                {
                    if (current_cluster)
                    {
                        current_cluster->initial_size =
                                list_len(current_cluster->entities);
                        if (!list_push_back(SpeciesPtr,
                                            clusters.get(),
                                            current_cluster.get()))
                        {
                            throw std::runtime_error("list_push_back failed");
                        }
                        current_cluster.release();
                    }
                    current_cluster.reset(CreateSpecies(0));
                    if (!current_cluster)
                    {
                        throw std::bad_alloc();
                    }
                    ++current_cluster_id;

                    auto new_entity = copy_entity(obj.entity, chr_size);
                    if (!list_push_back(EntityPtr,
                                        current_cluster->entities,
                                        new_entity.get()))
                    {
                        throw std::runtime_error("list_push_back failed");
                    }
                    new_entity.release();
                    if (cluster_labels)
                    {
                        (*cluster_labels)[obj.index] = current_cluster_id;
                    }
                }
                else
                {
                    // Noise object
                    if (noise_entities)
                    {
                        auto new_entity = copy_entity(obj.entity, chr_size);
                        if (!list_push_back(EntityPtr,
                                            noise_entities,
                                            new_entity.get()))
                        {
                            throw std::runtime_error("list_push_back failed");
                        }
                        new_entity.release();
                    }
                }
            }
            else
            {
                if (current_cluster)
                {
                    auto new_entity = copy_entity(obj.entity, chr_size);
                    if (!list_push_back(EntityPtr,
                                        current_cluster->entities,
                                        new_entity.get()))
                    {
                        throw std::runtime_error("pushBack failed");
                    }
                    new_entity.release();
                    if (cluster_labels)
                    {
                        (*cluster_labels)[obj.index] = current_cluster_id;
                    }
                }
                else
                {
                    // Noise object
                    if (noise_entities)
                    {
                        auto new_entity = copy_entity(obj.entity, chr_size);
                        if (!list_push_back(EntityPtr,
                                            noise_entities,
                                            new_entity.get()))
                        {
                            throw std::runtime_error("list_push_back failed");
                        }
                        new_entity.release();
                    }
                }
            }
        }
        if (current_cluster)
        {
            current_cluster->initial_size =
                    list_len(current_cluster->entities);
            if (!list_push_back(SpeciesPtr,
                                clusters.get(),
                                current_cluster.get()))
            {
                throw std::runtime_error("pushBack failed");
            }
            current_cluster.release();
        }
        return clusters.release();
    }
}

LIST_TYPE(SpeciesPtr) OPTICSClustering(World* world,
                                       LIST_TYPE(EntityPtr) new_entities,
                                       double eps, size_t min_pts,
                                       LIST_TYPE(EntityPtr) noise_entities)
{
    try
    {
        std::vector<OpticsObject> objects =
                extract_optics_objects(world->population, new_entities);
        auto cluster_ordering = optics(std::move(objects),
                                       world->chr_size, eps, min_pts);
        return extract_clusters(world->chr_size, cluster_ordering, eps,
                                nullptr, noise_entities);
    }
    catch (const std::exception& exc)
    {
        std::cerr << exc.what() << std::endl;
        return nullptr;
    }
}

std::vector<size_t> optics_clustering(
        const std::vector< std::vector<double> >& points,
        size_t chr_size, double eps, size_t min_pts)
{
    std::vector< std::unique_ptr<Entity, decltype(&DestroyEntity)> > entities;
    entities.reserve(points.size());

    std::vector<OpticsObject> objects =
            extract_optics_objects(points, entities);

    auto cluster_ordering = optics(std::move(objects), chr_size, eps, min_pts);

    std::vector<size_t> cluster_labels(points.size(), 0);
    std::unique_ptr<std::remove_pointer<LIST_TYPE(SpeciesPtr)>::type,
            decltype(&DestroyPopulation)>
            species_list(
            extract_clusters(chr_size, cluster_ordering, eps, &cluster_labels),
            &DestroyPopulation);
    if (!species_list)
    {
        throw std::runtime_error("Failed to extract clusters");
    }
    return cluster_labels;
}

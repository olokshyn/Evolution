//
// Created by Oleg on 9/18/16.
//

#include "TestsCommon.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <utility>
#include <memory>
#include <type_traits>

static double random_func(double* x, int n)
{
    return getRand(0.0, 1.0);
}

const Objective random_objective = {
        random_func,
        0.0,
        1.0
};

namespace
{

    std::map< size_t, std::unordered_set<size_t> > cluster_labels_to_clusters(
            const std::vector<size_t>& cluster_labels)
    {
        // Reorganize cluster_labels so every label will denote
        // a set of object`s indexes which it contains
        std::map< size_t, std::unordered_set<size_t> > clusters;
        for (size_t index = 0; index != cluster_labels.size(); ++index)
        {
            clusters[cluster_labels[index]].insert(index);
        }
        return clusters;
    };

}

Entity* MockCreateEntity(size_t chr_size, const Objective* obj)
{
    std::unique_ptr<Entity, decltype(&DestroyEntity)>
            new_entity(CreateEntity(chr_size), &DestroyEntity);
    if (!new_entity)
    {
        return nullptr;
    }
    for (size_t j = 0; j != chr_size; ++j)
    {
        if (obj)
        {
            new_entity->chr[j] = getRand(obj->min, obj->max);
        }
        else
        {
            new_entity->chr[j] = getRand(0, 1);
        }
    }
    new_entity->fitness = obj ? obj->func(new_entity->chr, (int)chr_size) : 0.0;
    new_entity->old = true;

    return new_entity.release();

}

LIST_TYPE(EntityPtr) MockCreateEntities(size_t number,
                                        size_t chr_size,
                                        const Objective* obj)
{
    std::unique_ptr<
            std::remove_pointer<LIST_TYPE(EntityPtr)>::type,
            decltype(&DestroyEntitiesList)>
            entities(list_create(EntityPtr), &DestroyEntitiesList);
    if (!entities)
    {
        return nullptr;
    }

    for (size_t i = 0; i != number; ++i)
    {
        std::unique_ptr<Entity, decltype(&DestroyEntity)>
                entity(MockCreateEntity(chr_size, obj), &DestroyEntity);
        if (!entity)
        {
            return nullptr;
        }
        if (!list_push_back(EntityPtr, entities.get(), entity.get()))
        {
            return nullptr;
        }
        entity.release();
    }
    return entities.release();
}

Species* MockCreateSpecies(size_t size, size_t chr_size, const Objective* obj)
{
    std::unique_ptr<Species, decltype(&DestroySpecies)>
            new_species(CreateSpecies(size), &DestroySpecies);
    if (!new_species)
    {
        return nullptr;
    }

    for (size_t i = 0; i != size; ++i)
    {
        std::unique_ptr<Entity, decltype(&DestroyEntity)>
                new_entity(MockCreateEntity(chr_size, obj), &DestroyEntity);
        if (!new_entity)
        {
            return nullptr;
        }
        if (!list_push_back(EntityPtr, new_species->entities, new_entity.get()))
        {
            return nullptr;
        }
        new_entity.release();
    }

    return new_species.release();
}

std::vector< std::vector<double> > read_points(const std::string& filename)
{
    std::ifstream points_file(filename, std::ios::in);
    if (!points_file.is_open())
    {
        throw std::runtime_error("Failed to open " + filename);
    }

    std::vector< std::vector<double> > points;

    std::string line;
    while (std::getline(points_file, line))
    {
        if (line.empty() || line[0] == '%')
        {
            continue;
        }
        std::vector<double> point;
        std::stringstream line_stream(line);
        std::istream_iterator<double> stream_iter(line_stream);
        // skip the first number in each line because it is a point number
        std::copy(++stream_iter,
                  std::istream_iterator<double>(),
                  std::back_inserter(point));
        points.emplace_back(std::move(point));
    }

    return points;
}

std::vector<size_t> read_cluster_labels(const std::string& filename)
{
    std::ifstream labels_file(filename, std::ios::in);
    if (!labels_file.is_open())
    {
        throw std::runtime_error("Failed to open " + filename);
    }

    std::vector<size_t> labels;

    std::string line;
    while (std::getline(labels_file, line))
    {
        if (line.empty() || line[0] == '%')
        {
            continue;
        }
        size_t label;
        std::stringstream line_stream(line);
        // skip the first number in each line because it is a point number
        line_stream >> label >> label;
        labels.push_back(label);
    }

    return labels;
}

size_t count_erroneously_clustered_points(
        const std::vector<size_t>& found_cluster_labels,
        const std::vector<size_t>& cluster_labels)
{
    auto found_clusters = cluster_labels_to_clusters(found_cluster_labels);
    auto clusters = cluster_labels_to_clusters(cluster_labels);

    // For every cluster find the biggest found_cluster that matches it
    std::map<size_t, size_t> clusters_matched_count;
    for (const auto& cluster : clusters)
    {
        clusters_matched_count[cluster.first] = 0;

        std::map<size_t, size_t> found_cluster_matches;
        for (const auto& found_cluster : found_clusters)
        {
            if (found_cluster.first == 0)
            {
                continue;
            }
            found_cluster_matches[found_cluster.first] = 0;
            for (size_t obj_index : found_cluster.second)
            {
                if (cluster.second.find(obj_index) != cluster.second.end())
                {
                    found_cluster_matches[found_cluster.first] += 1;
                }
            }
        }
        auto best_match_iter = std::max_element(
                found_cluster_matches.begin(),
                found_cluster_matches.end(),
                [](const std::pair<const size_t, size_t>& match_a,
                   const std::pair<const size_t,
                           size_t>& match_b) -> bool {
                    return match_a.second
                           < match_b.second;
                });
        if (best_match_iter == found_cluster_matches.end())
        {
            continue;
        }
        clusters_matched_count[cluster.first] = best_match_iter->second;
    }

    size_t erroneously_clustered_points_count = 0;
    for (const auto& cluster_matched_count : clusters_matched_count)
    {
        erroneously_clustered_points_count +=
                clusters[cluster_matched_count.first].size()
                - cluster_matched_count.second;
    }

    return erroneously_clustered_points_count;
}

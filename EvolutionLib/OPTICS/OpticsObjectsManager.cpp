//
// Created by Oleg on 10/7/17.
//

#include <algorithm>
#include <stdexcept>

#include "OpticsObjectsManager.h"

extern "C"
{

#include "Common.h"

}

OpticsObjectsManager::OpticsObjectsManager(
        std::vector<OpticsObject>&& objects, size_t chr_size)
        : m_objects(std::move(objects)),
          m_chr_size(chr_size)
{
    m_distance_matrix.resize(m_objects.size());
    for (size_t i = 0; i != m_objects.size(); ++i)
    {
        m_distance_matrix[i].resize(m_objects.size());
    }
    for (size_t i = 0; i != m_objects.size(); ++i)
    {
        for (size_t j = 0; j != m_objects.size(); ++j)
        {
            if (i == j)
            {
                m_distance_matrix[i][j] = 0.0;
            }
            else
            {
                m_distance_matrix[i][j] =
                        EuclidMeasure(m_objects[i].entity->chr,
                                      m_objects[j].entity->chr,
                                      m_chr_size);
            }
        }
    }
}

std::vector<size_t> OpticsObjectsManager::find_neighbor_indexes(
        const OpticsObject& object, double eps) const
{
    check_object_index(object.index);

    std::vector<size_t> neighbor_object_indexes;
    for (size_t index = 0; index != m_objects.size(); ++index)
    {
        if (index == object.index)
        {
            continue;
        }
        if (m_distance_matrix[object.index][index] <= eps)
        {
            neighbor_object_indexes.push_back(index);
        }
    }
    return neighbor_object_indexes;
}

OpticsObject& OpticsObjectsManager::object(size_t object_index)
{
    check_object_index(object_index);

    return m_objects[object_index];
}

void OpticsObjectsManager::set_core_distance(OpticsObject& object,
                                             const std::vector<size_t>& neighbor_indexes,
                                             double eps, size_t min_pts)
{
    check_object_index(object.index);

    if (neighbor_indexes.size() >= min_pts)
    {
        std::vector<double> object_distances = m_distance_matrix[object.index];
        sort(object_distances.begin(), object_distances.end());
        object.core_distance = object_distances[min_pts];
    }
    else
    {
        object.core_distance = INVALID_DISTANCE;
    }
}

void OpticsObjectsManager::set_reachability_distance(const OpticsObject& core_object,
                                                     OpticsObject& object)
{
    check_object_index(core_object.index);
    check_object_index(object.index);

    if (is_distance_valid(core_object.core_distance))
    {
        object.reachability_distance =
                std::max(core_object.core_distance,
                         m_distance_matrix[core_object.index][object.index]);
    }
    else
    {
        object.reachability_distance = INVALID_DISTANCE;
    }
}

double OpticsObjectsManager::distance(
        size_t object1_index, size_t object2_index) const
{
    check_object_index(object1_index);
    check_object_index(object2_index);

    return m_distance_matrix[object1_index][object2_index];
}

size_t OpticsObjectsManager::size() const
{
    return m_objects.size();
}

void OpticsObjectsManager::check_object_index(size_t object_index) const
{
    if (object_index >= m_objects.size())
    {
        throw std::out_of_range("Entity index is out of range");
    }
}

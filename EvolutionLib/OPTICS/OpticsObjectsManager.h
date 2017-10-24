//
// Created by Oleg on 10/7/17.
//

#ifndef EVOLUTION_OPTICSOBJECTSMANAGER_H
#define EVOLUTION_OPTICSOBJECTSMANAGER_H

#include <vector>

#include "OpticsObject.h"


class OpticsObjectsManager
{
public:
    OpticsObjectsManager(std::vector<OpticsObject>&& objects, size_t chr_size);

    OpticsObject& object(size_t object_index);


    std::vector<size_t> find_neighbor_indexes(const OpticsObject& object,
                                              double eps) const;

    void set_core_distance(OpticsObject& object,
                           const std::vector<size_t>& neighbor_indexes,
                           double eps, size_t min_pts);

    void set_reachability_distance(const OpticsObject& core_object,
                                   OpticsObject& object);

    double distance(size_t object1_index, size_t object2_index) const;

    size_t size() const;

private:
    void check_object_index(size_t object_index) const;

private:
    std::vector<OpticsObject> m_objects;
    std::vector<std::vector<double>> m_distance_matrix;
    size_t m_chr_size;
};


#endif //EVOLUTION_OPTICSOBJECTSMANAGER_H

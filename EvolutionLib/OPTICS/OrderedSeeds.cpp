//
// Created by Oleg on 10/7/17.
//

#include <cassert>

#include <algorithm>

#include "OrderedSeeds.h"


void OrderedSeeds::update(OpticsObjectsManager& objects_manager,
                          std::vector<size_t>& neighbor_indexes,
                          const OpticsObject& center_object)
{
    assert(is_distance_valid(center_object.core_distance));

    for (size_t neighbor_index : neighbor_indexes)
    {
        OpticsObject& obj = objects_manager.object(neighbor_index);
        if (obj.processed)
        {
            continue;
        }

        double reachability_dist =
                std::max(center_object.core_distance,
                         objects_manager.distance(center_object.index,
                                                  neighbor_index));
        if (!is_distance_valid(obj.reachability_distance))
        {
            obj.reachability_distance = reachability_dist;
            m_queue.add(obj.index, reachability_dist);
        }
        else if (reachability_dist < obj.reachability_distance)
        {
            obj.reachability_distance = reachability_dist;
            m_queue.update(obj.index, reachability_dist);
        }
    }
}

size_t OrderedSeeds::pop_next()
{
    return m_queue.pop_next();
}

bool OrderedSeeds::empty() const
{
    return m_queue.empty();
}

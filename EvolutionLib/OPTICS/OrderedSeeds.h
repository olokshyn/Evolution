//
// Created by Oleg on 10/7/17.
//

#ifndef EVOLUTION_ORDEREDSEEDS_H
#define EVOLUTION_ORDEREDSEEDS_H

#include <vector>

#include "OpticsObjectsManager.h"
#include "PrioritizedQueue.h"


class OrderedSeeds
{
public:
    void update(OpticsObjectsManager& objects_manager,
                std::vector<size_t>& neighbor_indexes,
                const OpticsObject& center_object);

    size_t pop_next();

    bool empty() const;

private:
    PrioritizedQueue<size_t, double> m_queue;
};


#endif //EVOLUTION_ORDEREDSEEDS_H

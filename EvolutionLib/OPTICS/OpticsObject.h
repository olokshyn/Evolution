//
// Created by Oleg on 10/7/17.
//

#ifndef EVOLUTION_OPTICSOBJECT_H
#define EVOLUTION_OPTICSOBJECT_H

extern "C"
{

#include "Entity/Entity.h"

};

static const double INVALID_DISTANCE = -1.0;
inline bool is_distance_valid(double distance)
{
    return distance >= 0.0;
}

struct OpticsObject
{
    OpticsObject(size_t index, Entity* entity)
            : index(index),
              entity(entity),
              core_distance(INVALID_DISTANCE),
              reachability_distance(INVALID_DISTANCE),
              processed(false)
    {
    }

    size_t index;
    const Entity* entity;
    double core_distance;
    double reachability_distance;
    bool processed;
};

#endif //EVOLUTION_OPTICSOBJECT_H

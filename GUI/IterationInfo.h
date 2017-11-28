//
// Created by Oleg on 11/22/17.
//

#ifndef GUI_ITERATIONINFO_H
#define GUI_ITERATIONINFO_H

#include <QMetaType>

struct IterationInfo
{
    size_t generation_number = 0;
    size_t world_size = 0;
    std::vector<size_t> species;
    std::vector<double> fitnesses;
    size_t new_entities_count = 0;
    size_t entities_died = 0;
    size_t species_died = 0;
};

Q_DECLARE_METATYPE(IterationInfo)

#endif //GUI_ITERATIONINFO_H

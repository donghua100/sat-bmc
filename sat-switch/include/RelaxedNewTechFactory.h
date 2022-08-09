//
// Created by guishuo on 2021/10/9.
//

#ifndef INV_SEARCH_RELAXEDNEWTECHFACTORY_H
#define INV_SEARCH_RELAXEDNEWTECHFACTORY_H

#include "SatDefs.h"
#include "sat-switch/relaxedNewTech/RelaxedNewTechSolver.h"

namespace sat{
    class RelaxedNewTechFactory{
    public:
        static SatSolver create();
    };
}

#endif //INV_SEARCH_RELAXEDNEWTECHFACTORY_H

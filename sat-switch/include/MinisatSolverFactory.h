//
// Created by 玥辰 on 2021/10/1.
//

#ifndef INV_SEARCH_MINISATSOLVERFACTORY_H
#define INV_SEARCH_MINISATSOLVERFACTORY_H

#include "SatDefs.h"
#include "sat-switch/minisat/MinisatSolver.h"

namespace sat {
    class MinisatSolverFactory {
    public:
        static SatSolver create();
    };
}


#endif //INV_SEARCH_MINISATSOLVERFACTORY_H

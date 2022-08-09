//
// Created by 玥辰 on 2021/10/2.
//

#ifndef INV_SEARCH_AVAILABLESOLVERS_H
#define INV_SEARCH_AVAILABLESOLVERS_H

#include "sat.h"


namespace sat {
    SatSolver createSolver(SolverEnum se);
}


#endif //INV_SEARCH_AVAILABLESOLVERS_H

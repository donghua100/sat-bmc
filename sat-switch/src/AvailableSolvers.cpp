//
// Created by 玥辰 on 2021/10/2.
//

#include "AvailableSolvers.h"
#include "MinisatSolverFactory.h"
#include "RelaxedNewTechFactory.h"

namespace sat {
    SatSolver createSolver(SolverEnum se) {
        SatSolver solver;
        switch (se) {
            case MINISAT:
                solver = MinisatSolverFactory::create();
                break;
//            case RELAXED_NEW_TECH:
//                solver = RelaxedNewTechFactory::create();
//                break;
            default:
                throw SatException("Unhandled solver enum");
        }
        return solver;
    }

}
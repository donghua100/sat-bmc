//
// Created by guishuo on 2021/10/9.
//

#include "MinisatSolverFactory.h"


using namespace sat;

SatSolver MinisatSolverFactory::create() {
    SatSolver solver = std::make_shared<MinisatSolver>();
    return solver;
}
//
// Created by guishuo on 2021/10/9.
//
#include "RelaxedNewTechFactory.h"
using namespace sat;

SatSolver RelaxedNewTechFactory::create() {
    SatSolver solver = std::make_shared<RelaxedNewTechSolver>();
    return solver;
}
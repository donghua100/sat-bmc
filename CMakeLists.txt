cmake_minimum_required(VERSION 3.10)
project(bmc)

SET(CMAKE_C_COMPILER /usr/bin/gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.
        ./sat-switch/include)


add_executable(bmc
        # main.cpp
        bmc.cpp
        aiger/aiger.c
#        aigtocnf/aig2cnf.cpp
        aigtocnf/aigtocnf.cpp

        minisat/utils/Options.cc
        minisat/utils/System.cc
        minisat/core/Solver.cc
        minisat/simp/SimpSolver.cc

        relaxedNewTech/SimpSolver.cc
        relaxedNewTech/Solver.cc
        relaxedNewTech/ccnr.cc
        relaxedNewTech/ipasir.cc


        sat-switch/include/AbsSatSolver.h
        sat-switch/include/SolverEnum.h
        sat-switch/include/SatDefs.h
        sat-switch/include/MinisatSolverFactory.h
        sat-switch/include/RelaxedNewTechFactory.h
        sat-switch/include/sat.h
        sat-switch/include/AvailableSolvers.h
        sat-switch/include/exceptions.h

        sat-switch/minisat/MinisatSolver.cpp
        sat-switch/minisat/MinisatSolver.h
        sat-switch/minisat/MinisatSolverFactory.cpp

#        sat-switch/relaxedNewTech/RelaxedNewTechSolver.cpp
#        sat-switch/relaxedNewTech/RelaxedNewTechSolver.h
#        sat-switch/relaxedNewTech/RelaxedNewTechFactory.cpp


        sat-switch/src/AbsSatSolver.cpp
        sat-switch/src/AvailableSolvers.cpp
  )

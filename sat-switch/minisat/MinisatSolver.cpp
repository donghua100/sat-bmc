//
// Created by guishuo on 2021/10/9.
//
#include "MinisatSolver.h"

using namespace sat;

int MinisatSolver::newVar() {
    // if minisat var start from zero,then +1
    return solver->newVar();
}

int MinisatSolver::nVars() {
    return solver->nVars()-1;
}

void MinisatSolver::addClause(int p)
{
    solver->addClause(int2lit(p));
}

void MinisatSolver::addClause(int p, int q) {
    solver->addClause(int2lit(p), int2lit(q));
}

void MinisatSolver::addClause(vector<int> &ps) {
    vec<Lit> vps;
    for(const int &x:ps) vps.push(int2lit(x));
    solver->addClause(vps);
}

bool MinisatSolver::solve() {
    return solver->solve();
}

bool MinisatSolver::solve(int assump) {
    return solver->solve(int2lit(assump));
}

bool MinisatSolver::solve(const vector<int> &assumps) {
    vec<Lit> vassumps;
    for(const int &x:assumps) vassumps.push(int2lit(x));
    return solver->solve(vassumps);
}

bool MinisatSolver::modelValue(int x) {
    return solver->modelValue(x) == l_True;
}
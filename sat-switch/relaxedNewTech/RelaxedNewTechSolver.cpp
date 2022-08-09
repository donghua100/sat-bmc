//
// Created by 玥辰 on 2021/10/3.
//

#include "RelaxedNewTechSolver.h"


using namespace sat;


int RelaxedNewTechSolver::newVar() {
    return ipasir_newVar(solver);
}

int RelaxedNewTechSolver::nVars()
{
    return ipasir_nVars(solver) - 1;
}

void RelaxedNewTechSolver::addClause(int p)
{
    ipasir_add(solver, p);
    ipasir_add(solver,0);
}

void RelaxedNewTechSolver::addClause(int p, int q) {
    ipasir_add(solver,p);
    ipasir_add(solver,q);
    ipasir_add(solver,0);
}

void RelaxedNewTechSolver::addClause(vector<int> &ps)
{
    for (auto x : ps)
    {
        ipasir_add(solver,x);
    }
    ipasir_add(solver,0);
}

bool RelaxedNewTechSolver::solve()
{
    // to do when the return value is undef,it may be sat or unsat
    // ipasir_solve solve whith resource limited
    // do a while stmt can prove it return 10 or 20 ?

    return ipasir_solve(solver) == 10;
}

bool RelaxedNewTechSolver::solve(int assump)
{
    ipasir_assume(solver,assump);
    return ipasir_solve(solver) == 10;

}

bool RelaxedNewTechSolver::solve(const vector<int> &assumps)
{
    for (auto x: assumps) // 1 2 3
    {
        ipasir_assume(solver, x);
    }
    return ipasir_solve(solver) == 10;
}



bool RelaxedNewTechSolver::modelValue(int p)
{
    return ipasir_val(solver,p) > 0;
}



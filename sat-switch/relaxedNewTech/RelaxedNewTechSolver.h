//
// Created by 玥辰 on 2021/10/3.
//

#ifndef INV_SEARCH_RELAXEDNEWTECHSOLVER_H
#define INV_SEARCH_RELAXEDNEWTECHSOLVER_H

#include "AbsSatSolver.h"
#include "relaxedNewTech/ipasir.h"


namespace sat {

    class RelaxedNewTechSolver : public AbsSatSolver {
    private:
        void * solver;

    public:
        RelaxedNewTechSolver() : AbsSatSolver(RELAXED_NEW_TECH), solver(ipasir_init())
        {
            ipasir_newVar(solver);
        }
        RelaxedNewTechSolver(const RelaxedNewTechSolver &) = delete;
        RelaxedNewTechSolver & operator=(const RelaxedNewTechSolver &) = delete;

        ~RelaxedNewTechSolver() override
        {
            ipasir_release(solver);
        }

        int newVar() override;

        int nVars() override;

        void addClause(int p) override;

        void addClause(int p, int q) override;

        void addClause(vector<int> &ps) override;


        bool solve(const vector<int> &assumps) override;
        bool solve(int assump) override;
        bool solve() override;


        bool modelValue(int x) override;


    };

}


#endif //INV_SEARCH_RELAXEDNEWTECHSOLVER_H

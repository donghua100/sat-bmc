//
// Created by guishuo on 2021/10/9.
//

#ifndef INV_SEARCH_MINISATSOLVER_H
#define INV_SEARCH_MINISATSOLVER_H
#include "AbsSatSolver.h"
#include "minisat/simp/SimpSolver.h"
using namespace Minisat;

namespace sat{
    class MinisatSolver:public AbsSatSolver{
    private:
        SimpSolver * solver;
    public:
        MinisatSolver() : AbsSatSolver(MINISAT), solver(new SimpSolver()) {
            solver->newVar();
        }
        MinisatSolver(const MinisatSolver &) = delete;
        MinisatSolver & operator=(const MinisatSolver &) = delete;

        ~MinisatSolver() override {
            delete solver;
        }
        int newVar() override;

        int nVars() override;

        void addClause(vector<int> &ps) override;

        void addClause(int p) override;

        void addClause(int p,int q) override;

        bool solve() override;

        bool solve(const vector<int> &assumps) override;

        bool solve(int assump) override;

        bool modelValue(int x) override;

        static Lit int2lit(int x)
        {
            return mkLit(abs(x),x<0);
        }

    };
}


#endif //INV_SEARCH_MINISATSOLVER_H

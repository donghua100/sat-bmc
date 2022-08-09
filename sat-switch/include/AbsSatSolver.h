//
// Created by guishuo on 2021/10/9.
//

#ifndef INV_SEARCH_ABSSOLVER_H
#define INV_SEARCH_ABSSOLVER_H

#include "SolverEnum.h"
#include "vector"

using namespace std;

namespace sat{
    class AbsSatSolver{
    private:
        SolverEnum solver_enum;
    public:
        explicit AbsSatSolver(SolverEnum se): solver_enum(se) {};

        virtual ~AbsSatSolver() = default;

        virtual int newVar() = 0;

        virtual int nVars() = 0;

        virtual void addClause(vector<int> &ps) = 0;

        virtual void addClause(int p) = 0;

        virtual void addClause(int p,int q) = 0;

        virtual bool solve() = 0;

        virtual bool solve(const vector<int> &assumps) = 0;

        virtual bool solve(int assump) = 0;

        virtual bool modelValue(int x) = 0;
    };
}

#endif //INV_SEARCH_ABSSOLVER_H

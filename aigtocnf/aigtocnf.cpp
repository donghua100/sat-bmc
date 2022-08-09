//
// Created by guishuo on 2021/10/24.
//

#include<iostream>

#include<vector>
#include<string>

extern "C" {
#include "aiger/aiger.h"
}
using namespace std;


void equal_and2cnf(int p,int l,int r,vector<vector<int> > &v)
{
    // p = l & r  : (-p,l) (-p,r) (p,-l,-r)
    vector<int> c;
    c.push_back(-p);
    c.push_back(l);
    v.push_back(c);
    c.clear();

    c.push_back(-p);
    c.push_back(r);
    v.push_back(c);
    c.clear();

    c.push_back(p);
    c.push_back(-l);
    c.push_back(-r);
    v.push_back(c);
    c.clear();
}

void equal2cnf(int l,int r,vector<vector<int> > &v)
{
    vector<int> c;
    c.push_back(l);
    c.push_back(-r);
    v.push_back(c);
    c.clear();

    c.push_back(-l);
    c.push_back(r);
    v.push_back(c);
    c.clear();

}

void aigtocnf(aiger * mgr,vector<int> &inputs,vector<int> &inits,
             vector<int>& bads,vector<int> &in_latchs,vector<int> &nxt_states,vector<vector<int>> &ands)
{
    for (int i = 0; i < mgr->num_inputs; i++)
    {
        unsigned int l = mgr->inputs[i].lit/2+1;
        inputs.push_back(l);
    }

    for (int i = 0; i < mgr->num_latches; i++)
    {
        unsigned int l = mgr->latches[i].lit/2+1;
        int nxt_l= mgr->latches[i].next;
        int nxt_s = (nxt_l%2)?-(nxt_l/2+1):(nxt_l/2+1);
        in_latchs.push_back(l);
        nxt_states.push_back(nxt_s);

        // reset = 0 初始化为0, 取 not ;reset = 1,初始化为1，即 -1，否则 undef 0
        if (mgr->latches[i].reset == 0) inits.push_back(-l);
        else if (mgr->latches[i].reset == 1) inits.push_back(l);
        else inits.push_back(-l);
    }


    for (int i = 0; i < mgr->num_ands; i++)
    {
        aiger_and *a = mgr->ands + i;
        int rhs0 = a->rhs0/2+1;
        int rhs1 = a->rhs1/2+1;

        if (a->rhs0%2) rhs0 = -rhs0;
        if (a->rhs1%2) rhs1 = -rhs1;

        equal_and2cnf(a->lhs/2+1,rhs0,rhs1,ands);

    }

    // vector<vector<int> >bad;
    if (mgr->num_bad == 0)
    {
        for (int i = 0; i < mgr->num_outputs; i++)
        {
            int v = mgr->outputs[i].lit/2+1;
            bads.push_back(v);
        }
    }
    for (int i = 0; i < mgr->num_bad; i++)
    {
        int v = mgr->bad[i].lit/2+1;
        bads.push_back(v);
    }

    // 暂时不考虑 C
    return ;

}
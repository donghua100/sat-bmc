#include<iostream>

#include<vector>
#include<string>
#include<map>
#include<set>
#include<unordered_map>
#include <utility>

extern "C" {
#include "aiger/aiger.h"
}
using namespace std;




// 1. 避免使用 minisat 的数据结构
// 2. 提取变量，偶数为正，奇数为负,变量从1 开始，-1 为 false, 1 为 true, 为常量；0 为 undef
// 3. 输入，latch，and lhs 为偶数
// 4. 为了区分 0 1，对变量值＋1；因此，0 1 对应 1，1 2 对应 2

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
// 3 = 1 and 2
// 5 = 3 and 4
//
void aig2cnf(aiger * mgr,vector<int> &inputs,vector<int> &init,vector<vector<int> >&trans,
            vector<vector<int> >& bad,vector<int> &cur_state,vector<int> &next_state)
{
    int start_idx = mgr->maxvar + 1;
    // vector<int> inputs;
    unordered_map<int,int> inputs2var;
    for (int i = 0; i < mgr->num_inputs; i++)
    {
        unsigned int l = mgr->inputs[i].lit/2+1;
        inputs.push_back(l);
        inputs2var[l] = l;

    }

    vector<int> in_latch;

    unordered_map<int,int> in_latch2var;
    unordered_map<int,int> next_state2var;
    //vector<int> init;
    vector<int> reset;
    for (int i = 0; i < mgr->num_latches; i++)
    {
        unsigned int l = mgr->latches[i].lit/2+1;
        in_latch.push_back(l);
        in_latch2var[l] = l;
        cur_state.push_back(l);
        next_state.push_back(start_idx+i+1);

        // reset = 0 初始化为0, 取 not ;reset = 1,初始化为1，即 -1，否则 undef 0

        if (mgr->latches[i].reset == 0) 
        {
            init.push_back(-l);
            reset.push_back(0);
        }
        else if (mgr->latches[i].reset == 1) 
        {
            init.push_back(l);
            reset.push_back(1);
        }
        else 
        {
            init.push_back(l);
            reset.push_back(mgr->latches[i].reset);
        }
    }

    unordered_map<int,vector<vector<int> > > ands_cnf;
    // unordered_map<int, set<int> > child;
    vector<int> vv_size;
    for (int i = 0; i < mgr->num_ands; i++)
    {
        aiger_and *a = mgr->ands + i;
        int rhs0 = a->rhs0/2+1;
        int rhs1 = a->rhs1/2+1;

        if (a->rhs0%2) rhs0 = -rhs0;
        if (a->rhs1%2) rhs1 = -rhs1;

        // auto & ch = child[a->lhs/2+1];
        // ch.insert(abs(rhs0));
        // ch.insert(abs(rhs1));
        // if (child.find(abs(rhs0))!=child.end())
        // {
        //     auto & l = child[abs(rhs0)];
        //     for (auto x :l) ch.insert(x);
        // }
        // if (child.find(abs(rhs1))!=child.end())
        // {
        //     auto & l = child[abs(rhs1)];
        //     for (auto x :l) ch.insert(x);
        // }

        auto & v = ands_cnf[a->lhs/2+1];
        equal_and2cnf(a->lhs/2+1,rhs0,rhs1,v);
        if (ands_cnf.find(abs(rhs0))!=ands_cnf.end())
        {
           const auto &vv = ands_cnf[abs(rhs0)];
           vv_size.push_back(vv.size());
           v.insert(v.end(),vv.begin(),vv.end());
        }
        if (ands_cnf.find(abs(rhs1))!=ands_cnf.end())
        {
            const auto & vv = ands_cnf[abs(rhs1)];
            v.insert(v.end(),vv.begin(),vv.end());
        }
        // if (i == 100)
        // {
        //     cout<<"size:\n";
        //     for (auto x :vv_size) cout<<x<<" ";
        //     cout<<"\n";
        // }
    }

    // vector<vector<int> > trans;
    for (int i = 0; i < mgr->num_latches; i++)
    {
        int curl = mgr->latches[i].next;
        int curs = curl/2+1;
        int nxts = next_state[i];
        if (curl == 0)
        {
            // aig 中 0 true，1 false
            // 1 = nxt
            // nxt
            // vector<vector<int> > t;
            equal2cnf(1,nxts,trans);
            // for (auto x:t) trans.push_back(x);
        }
        else if (curl == 1)
        {
            // -1 = nxt
            // vector<vector<int> > t;
            equal2cnf(-1,nxts,trans);
            // for (auto x:t) trans.push_back(x);
        }
        else 
        {
            if (curl%2)
            {
                // -curs = nxt
                // vector<vector<int> > t;
                equal2cnf(-curs,nxts,trans);
                // for (auto x:t) trans.push_back(x);
                if (ands_cnf.find(curs)!=ands_cnf.end())
                {
                    const auto &ands = ands_cnf[curs];
                    for (auto a:ands) trans.push_back(a);
                }
            }
            else 
            {
                // curs = nxt
                // vector<vector<int> > t;
                equal2cnf(curs,nxts,trans);
                // for (auto x:t) trans.push_back(x);
                if (ands_cnf.find(curs)!=ands_cnf.end())
                {
                    const auto &ands = ands_cnf[curs];
                    for (auto a:ands) trans.push_back(a);
                }
            }
        }
    }

    // vector<vector<int> >bad;
    if (mgr->num_bad == 0)
    {
        for (int i = 0; i < mgr->num_outputs; i++)
        {
            int l = mgr->outputs[i].lit;
            int v = l/2+1;
            if (l%2)
            {
                vector<int> c;
                c.push_back(-v);
                bad.push_back(c);
                c.clear();
                if (ands_cnf.find(v)!=ands_cnf.end())
                {
                    const auto & ands = ands_cnf[v];
                    for (auto a:ands) bad.push_back(a);
                }
            }
            else 
            {
                vector<int> c;
                c.push_back(v);
                bad.push_back(c);
                c.clear();
                if (ands_cnf.find(v)!=ands_cnf.end())
                {
                    const auto & ands = ands_cnf[v];
                    for (auto a:ands) bad.push_back(a);
                }
            }
        }    
    }
    for (int i = 0; i < mgr->num_bad; i++)
    {
        int l = mgr->bad[i].lit;
        int v = l/2+1;
        if (l%2)
        {
            vector<int> c;
            c.push_back(-v);
            bad.push_back(c);
            c.clear();
            if (ands_cnf.find(v)!=ands_cnf.end())
            {
               const auto & ands = ands_cnf[v];
                for (auto a:ands) bad.push_back(a);
            }
        }
        else 
        {
            vector<int> c;
            c.push_back(v);
            bad.push_back(c);
            c.clear();
            if (ands_cnf.find(v)!=ands_cnf.end())
            {
                const auto & ands = ands_cnf[v];
                for (auto a:ands) bad.push_back(a);
            }
        }
    }

    // 暂时不考虑 C
    return ;

}


// 记忆化搜索
//unordered_map<int,vector<vector<int>> > ans;
//unordered_map<int,pair<int,int>> node;
//vector<vector<int> >  dfs(unordered_map<int,vector<vector<int> > > &ands,int x,unordered_map<int,int> &leaf)
//{
//    if (ans.find(x)!=ans.end()) return ans[x];
//    auto & res = ans[x];
//    if (leaf.find(x)!=leaf.end())
//    {
//        return res;
//    }
//    else
//    {
//        res = ands[x];
//        int l = node[x].first;
//        int r = node[x].second;
//        auto dl = dfs(ands,l,leaf);
//        auto dr = dfs(ands,r,leaf);
//        for (auto x:dl) res.push_back(x);
//        for (auto x:dr) res.push_back(x);
//        return res;
//    }
//    return res;
//}



//void aigtocnf(aiger * mgr,vector<int> &inputs,vector<int> &init,vector<vector<int> >&trans,
//            vector<vector<int> >& bad,vector<int> &cur_state,vector<int> &next_state)
//{
//    int start_idx = mgr->maxvar + 1;
//    // vector<int> inputs;
//    unordered_map<int,int> inputs2var;
//    unordered_map<int,int> leaf;
//    for (int i = 0; i < mgr->num_inputs; i++)
//    {
//        unsigned int l = mgr->inputs[i].lit/2+1;
//        inputs.push_back(l);
//        inputs2var[l] = l;
//        leaf[l] = l;
//
//    }
//
//    vector<int> in_latch;
//
//    unordered_map<int,int> in_latch2var;
//    unordered_map<int,int> next_state2var;
//    //vector<int> init;
//    vector<int> reset;
//    for (int i = 0; i < mgr->num_latches; i++)
//    {
//        unsigned int l = mgr->latches[i].lit/2+1;
//        in_latch.push_back(l);
//        in_latch2var[l] = l;
//        leaf[l] = l;
//        cur_state.push_back(l);
//        next_state.push_back(start_idx+i+1);
//
//        // reset = 0 初始化为0, 取 not ;reset = 1,初始化为1，即 -1，否则 undef 0
//
//        if (mgr->latches[i].reset == 0)
//        {
//            init.push_back(-l);
//            reset.push_back(0);
//        }
//        else if (mgr->latches[i].reset == 1)
//        {
//            init.push_back(l);
//            reset.push_back(1);
//        }
//        else
//        {
//            init.push_back(l);
//            reset.push_back(mgr->latches[i].reset);
//        }
//    }
//
//    unordered_map<int,vector<vector<int> > > one_ands;
//
//    vector<int> vv_size;
//    for (int i = 0; i < mgr->num_ands; i++)
//    {
//        aiger_and *a = mgr->ands + i;
//        int rhs0 = a->rhs0/2+1;
//        int rhs1 = a->rhs1/2+1;
//
//        if (a->rhs0%2) rhs0 = -rhs0;
//        if (a->rhs1%2) rhs1 = -rhs1;
//
//        auto & one_and = one_ands[a->lhs/2+1];
//        auto & n = node[a->lhs/2+1];
//        n = make_pair(abs(rhs0),abs(rhs1));
//        equal_and2cnf(a->lhs/2+1,rhs0,rhs1,one_and);
//    }
//
//
//    for (int i = 0; i < mgr->num_latches; i++)
//    {
//        int curl = mgr->latches[i].next;
//        int curs = curl/2+1;
//        int nxts = next_state[i];
//        if (curl == 0)
//        {
//            // aig 中 0 true，1 false
//            // 1 = nxt
//            equal2cnf(1,nxts,trans);
//
//        }
//        else if (curl == 1)
//        {
//            // -1 = nxt
//            equal2cnf(-1,nxts,trans);
//        }
//        else
//        {
//            if (curl%2)
//            {
//                // -curs = nxt
//                equal2cnf(-curs,nxts,trans);
//                if (one_ands.find(curs)!=one_ands.end())
//                {
//                    const auto ands = dfs(one_ands,curs,leaf);
//                    for (auto a:ands) trans.push_back(a);
//                }
//            }
//            else
//            {
//                // curs = nxt
//                equal2cnf(curs,nxts,trans);
//                if (one_ands.find(curs)!=one_ands.end())
//                {
//                    const auto ands = dfs(one_ands,curs,leaf);
//                    for (auto a:ands) trans.push_back(a);
//                }
//            }
//        }
//    }
//
//    // vector<vector<int> >bad;
//    if (mgr->num_bad == 0)
//    {
//        for (int i = 0; i < mgr->num_outputs; i++)
//        {
//            int l = mgr->outputs[i].lit;
//            int v = l/2+1;
//            if (l%2)
//            {
//                vector<int> c;
//                c.push_back(-v);
//                bad.push_back(c);
//                c.clear();
//                if (one_ands.find(v)!=one_ands.end())
//                {
//                    const auto  ands = dfs(one_ands,v,leaf);
//                    for (auto a:ands) bad.push_back(a);
//                }
//            }
//            else
//            {
//                vector<int> c;
//                c.push_back(v);
//                bad.push_back(c);
//                c.clear();
//                if (one_ands.find(v)!=one_ands.end())
//                {
//                    const auto  ands = dfs(one_ands,v,leaf);
//                    for (auto a:ands) bad.push_back(a);
//                }
//            }
//        }
//    }
//    for (int i = 0; i < mgr->num_bad; i++)
//    {
//        int l = mgr->bad[i].lit;
//        int v = l/2+1;
//        if (l%2)
//        {
//            vector<int> c;
//            c.push_back(-v);
//            bad.push_back(c);
//            c.clear();
//            if (one_ands.find(v)!=one_ands.end())
//            {
//                const auto  ands = dfs(one_ands,v,leaf);
//                for (auto a:ands) bad.push_back(a);
//            }
//        }
//        else
//        {
//            vector<int> c;
//            c.push_back(v);
//            bad.push_back(c);
//            c.clear();
//            if (one_ands.find(v)!=one_ands.end())
//            {
//                const auto  ands = dfs(one_ands,v,leaf);
//                for (auto a:ands) bad.push_back(a);
//            }
//        }
//    }
//
//    // 暂时不考虑 C
//    return ;
//
//}
//
// Created by guishuo on 2021/10/24.
//

#include <cstdio>
#include<iostream>
#include<chrono>
#include<ctime>
#include<vector>
#include<string>
#include <cstring>
#include <dirent.h>
#include<unordered_map>
#include "minisat/simp/SimpSolver.h"
#include "AvailableSolvers.h"

#include <unistd.h>
#include <getopt.h>
extern "C"
{
#include"aiger/aiger.h"
}

using namespace std;
using namespace Minisat;

int cur_idx = 0,start_idx = 1,vars_nums = 0;
unordered_map<int,int> cur2nxt;
void update_cur2nxt()
{
    cur2nxt.clear();
    // int tmp_start_idx = start_idx,tmp_end_idx = end_idx;
    // var_nums 和 bad states 个数相关，
    // 维护idx的原因：1) 生成新的state
    //              2) 求解器newVar 的需要
    //              3) 如果将newVar 的变量当作变量则不需要idx？还是需要记录那些是state，那些是act
    int end_idx = start_idx + vars_nums;
    int tmp_start_idx = cur_idx;
    for (int i = start_idx; i <= end_idx; i++)
    {
        cur2nxt[i] = cur_idx++;
    }
    start_idx = tmp_start_idx;
}

Lit int2lit(int x){return mkLit(abs(x),x < 0);}
void vec2clause(const vector<int> &v,vec<Lit> &c)
{
    for (const auto & x:v) c.push(int2lit(x));
}


void update_vars(vector<int> &cur_vars,vector<int> &new_vars)
{
    new_vars.clear();
    for (const auto &x:cur_vars)
    {
        int sign = x > 0?1:-1;
        new_vars.push_back(cur2nxt[abs(x)]*sign);
    }
}
void update_vars(vector<vector<int> > &cur_vars,vector<vector<int> > &new_vars)
{
    new_vars.clear();
    for (auto &v:cur_vars)
    {
        vector<int> t;
        update_vars(v,t);
        new_vars.emplace_back(t);
    }
}
void printvec(vector<int> &v)
{
    for (const auto &x:v)
    {
        cout<<x<<" ";
    }
    cout<<endl;
}


bool bmc(vector<int>&inputs,vector<int> &in_latch,vector<int> &inits,vector<int> &out_latch,
         vector<vector<int>> &ands,vector<int> &bads,int n)
{
    auto start = chrono::high_resolution_clock::now();
    Solver *S;
    S = new Solver();
    while (S->nVars()<=cur_idx) S->newVar();
    for (const auto &x:inits) S->addClause(int2lit(x));
    // printvec(inits);
    for(const auto &a:ands)
    {
        vec<Lit> c;
        vec2clause(a,c);
        S->addClause(c);
    }
    vec<Lit> v_act;
    for (const auto &x:bads)
    {
        vec<Lit> c;
        c.push(int2lit(x));
        Lit act = int2lit(cur_idx++);
        S->newVar();
        c.push(~act);
        S->addClause(c);
        v_act.push(act);
    }
    bool sat = S->solve(v_act);
    v_act.clear();
    if (sat)
    {
        cout<<"find in init state!"<<endl;
        for(int i = 1; i < cur_idx && i < 15; i++)
        {
            cout<<i<<" = "<<(S->modelValue(i) == l_True)<<endl;
        }
        return true;
    }
    vector<vector<int>> input_at_time_t;
    for (int i = 1; i <= n; i++)
    {
        update_cur2nxt();
        while (S->nVars()<=cur_idx) S->newVar();
        vector<int> new_in_latch;
        update_vars(in_latch,new_in_latch);
        for (int j = 0; j < in_latch.size(); j++)
        {
            // x = out_latch[i]
            // x = y (-x,y) and (x,-y)
            int x = new_in_latch[j];
            int y = out_latch[j];
            S->addClause(int2lit(-x), int2lit(y));
            S->addClause(int2lit(x), int2lit(-y));
        }
        in_latch = new_in_latch;
        vector<int> new_out_latch;
        update_vars(out_latch,new_out_latch);
        out_latch = new_out_latch;

        vector<vector<int>> new_ands;
        update_vars(ands,new_ands);
        ands = new_ands;
        for(const auto &a:ands)
        {
            vec<Lit> c;
            vec2clause(a,c);
            S->addClause(c);
        }

        vector<int> new_bads;
        update_vars(bads,new_bads);
        bads = new_bads;
        v_act.clear();
        for (const auto &x:bads)
        {
            vec<Lit> c;
            c.push(int2lit(x));
            Lit act = int2lit(cur_idx++);
            S->newVar();
            c.push(~act);
            S->addClause(c);
            v_act.push(act);
        }
        input_at_time_t.emplace_back(inputs);
        vector<int> new_inputs;
        update_vars(inputs,new_inputs);
        inputs = new_inputs;
        bool Sat = S->solve(v_act);
        v_act.clear();
        if (Sat)
        {
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> diff = end - start;
            cout<<"find int "<<i<<"steps, using time "<<diff.count()<<" s"<<endl;
            for(int j = 1; j < cur_idx && j< 15; j++)
            {
                cout<<j<<" = "<<(S->modelValue(j) == l_True)<<endl;
            }
            cout<<"-----------------------------\n";
            int t = 0;
            for (auto &v :input_at_time_t)
            {
                t++;
                cout<<"at time "<<t<<" : ";
                for (auto x:v)
                {

                    cout<<x<<" = "<<(S->modelValue(x) == l_True)<<",";
                }
                cout<<endl;
            }
            return true;
        }
    }
    cout<<"can't find!"<<endl;
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    cout<<"using time "<<diff.count()<<" s"<<endl;
    return false;
}

void aigtocnf(aiger * mgr,vector<int> &inputs,vector<int> &inits,
              vector<int>& bads,vector<int> &in_latch,vector<int> &out_latch,vector<vector<int>> &ands);

static void usage(int argc,char *argv[]){
	printf("Usage: %s [OPTIONS...] -f [aigfile]\n\n",argv[0]);		
	printf("\t-k,--kamx		run bmc k steps\n");
	exit(0);
}

static char *aigfile = NULL;
static int kmax = 1000;
static int parser_args(int argc,char *argv[]){
	if (argc < 2) usage(argc,argv);
	const struct option table[] = {
		{"file",required_argument,NULL,'f'},
		{"kmax",required_argument,NULL,'k'},
		{0,0,NULL,0},
	};
	int o;
	while((o = getopt_long(argc,argv,"f:-k:",table,NULL))!=-1){
		switch(o){
			case 'f':
				aigfile=optarg;
				break;
			case 'k':
				kmax=stoi(optarg);
				break;
			default:
				usage(argc,argv);
		}
	}
	return 0;

}

int main(int argc,char *argv[])
{
	parser_args(argc,argv);
    aiger *aig = NULL;
    aig = aiger_init();
    const char * path = aigfile;
	printf("%s\n", path);
    const auto *error = aiger_open_and_read_from_file(aig, path);
    if(error) fprintf (stderr, "*** %s: %s\n", "<stdin>", error);
    vector<int> inputs,inits,bads,in_latch,out_latch;
    vector<vector<int>> ands;
    aigtocnf(aig,inputs,inits,bads,in_latch,out_latch,ands);
    start_idx = 1;
    vars_nums = aig->maxvar;
    cur_idx = vars_nums + 1 + 1;
    bmc(inputs,in_latch,inits,out_latch,ands,bads,kmax);
    return 0;
}

// to do the constraint
// when the latch is lager than 1

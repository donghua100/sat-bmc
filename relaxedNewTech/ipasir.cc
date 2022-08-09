
#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS
#include "SimpSolver.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>

using namespace std;
using namespace RelaxedNewTech;

extern "C" {
	static const char * sig = "Relaxed_Inc v1.0";
	#include <sys/resource.h>
	#include <sys/time.h>
	static double getime (void) {
		struct rusage u;
		double res;
		if (getrusage (RUSAGE_SELF, &u)) return 0;
		res  = u.ru_utime.tv_sec + 1e-6 * u.ru_utime.tv_usec;
		res += u.ru_stime.tv_sec + 1e-6 * u.ru_stime.tv_usec;
		return res;
	}
};


class IPAsirRelaxed : public Solver {
private:
    vec<Lit> tmp_assumptions;
	vec<Lit> tmp_add_clause;
    int 	 fail_map_sz; 
	unsigned char * fail_map; 
	int      last_result;
    unsigned long long solver_calls;
	double 	 last_runTime = 0;
    
	void reset_fail () { 
		if (fail_map) {
			delete 	[] fail_map;
			fail_map 	= 0;
			fail_map_sz	= 0;
		}
	}
	
	Lit  transLit (int diamcs_lit) { 
		while (abs(diamcs_lit) > nVars()) newVar ();
		return mkLit (Var( abs(diamcs_lit)-1), (diamcs_lit<0));
	}
	
	void analyze_fail () {
		if(fail_map) delete[] fail_map;
		fail_map = new unsigned char [fail_map_sz = nVars ()];
		memset (fail_map, 0, fail_map_sz);
		int confl_sz = conflict.size ();
		for (int i = 0; i < confl_sz; ++i) {
			int tmp_var = var (conflict[i]);
			fail_map[tmp_var] = 1;
		}
	}
  
public:

  	IPAsirRelaxed () : 
  	  fail_map_sz 	(0) 
  	, fail_map 		(0)
	, last_result 	(0)
	, solver_calls 	(0) 
	{
    	verbosity = 0;
  	}

 	~IPAsirRelaxed () { 
		reset_fail (); 
	}
  
	void add (int dimacs_lit) {
		last_result = 0;
		if (dimacs_lit != 0){
			tmp_add_clause.push (transLit (dimacs_lit));
			
		}else{
			addClause_ (tmp_add_clause);
			tmp_add_clause.clear ();
		}
	}

	void assume (int dimacs_lit) {
		last_result = 0;
		tmp_assumptions.push (transLit (dimacs_lit));
	}

	int call_solver () {
		//ok = true;
		solver_calls++;
		// double start_time = getime();
		// if(tmp_assumptions.size() > 0){
		// 	printf("assumptions.size = %d\n",tmp_assumptions.size());
		// }
		lbool res = solveLimited (tmp_assumptions);
		// last_runTime = getime() - start_time;
		// printf("c inner runtime : %f\n" ,last_runTime);

		tmp_assumptions.clear();
		if(res==l_Undef){
			last_result = 0;
		}else{
			if(res == l_True){
				last_result = 10;
			}else{
				last_result = 20;
				analyze_fail ();
			}
		}
		// cout<<"test:"<<last_result<<endl;
		// cout<<(solved_by_ls?"T":"F")<<endl;
		// last_result = ((res == l_Undef) ? 0 : (res == l_True ? 10 : 20) );
		return last_result;
	}

	int model_value (int dimacs_lit) {
		if (last_result != 10) return 0;
		lbool res = modelValue (transLit (dimacs_lit));
		return (res == l_True) ? dimacs_lit : -dimacs_lit;
	}

	int is_failed_reason (int dimacs_lit) {
		if(last_result != 20) return 0;
		int tmp = var (transLit (dimacs_lit));
		return fail_map[tmp];
	}

	void status () {
		printf("c Release info\n");
		printf("c Res = %d, cpuTime = %.2f s\n",last_result,last_runTime);
		printf("c Final nVars, nClauses, nLearnts = %d, %d , %d\n",
			nVars(),
			nClauses(),
			nLearnts());
		fflush (stdout);
	}

	// void set_callback_terminate(void* cut_state, int(*callback_fun)(void *state)){
	// 	terminate_state = cut_state;
	// 	terminate_outer = callback_fun;
	// }
	
	// void set_callback_learnts(void* tmp_learner, int max_length, void (*callback_learn)(void * leaner, int *l_clause)){
	// 	learner = tmp_learner;
	// 	save_max_len = max_length;
	// 	set_learner = callback_learn;
	// }

};

extern "C" {
#include "ipasir.h"
IPAsirRelaxed * import (void * s) { return (IPAsirRelaxed*) s; }
const char * ipasir_signature () { return sig; }
void * ipasir_init () { return new IPAsirRelaxed (); }
void ipasir_release (void * s) { 
	// import (s)->status ();
	delete import (s); 
}
int  ipasir_solve (void * s) { return import (s)->call_solver (); }
void ipasir_add (void * s, int l) { import (s)->add (l); }
void ipasir_assume (void * s, int l) { import (s)->assume (l); }
int  ipasir_val (void * s, int l) { return import (s)->model_value (l); }
int  ipasir_failed (void * s, int l) { return import (s)->is_failed_reason (l); }
//void ipasir_set_terminate (void * s, void * state, int (*callback)(void * state)) { import(s)->set_callback_terminate(state, callback); }
//void ipasir_set_learn (void * s, void * state, int max_length, void (*learn)(void * state, int * clause)) { import(s)->set_callback_learnts(state, max_length, learn); }

// add by guishuo
int ipasir_newVar(void *s) {return import(s)->newVar();}
int ipasit_nVars(void *s) {return import(s)->nVars();}

};

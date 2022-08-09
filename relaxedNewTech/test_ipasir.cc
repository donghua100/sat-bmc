#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
extern "C"{
    #include "ipasir.h"
};
using namespace std;
int val[1000];
int n = 7;

static int ph (int p, int h) {
	// variable start from 1.
	// ph(p,h) : pigenon p assigned in hole h 
	return 1 + h * (n+1) + p;
}



static void load_bones (void *solver, string name){
	ifstream fin(name.c_str());
	string line;
	while(getline(fin,line)){
		if(line.size()==0) continue;
		if(line[0] == 'p' || line[0] == 'c') continue;
		stringstream ss(line);
		int lit;
		while(ss>>lit){
			if(lit==0) break;
			ipasir_add(solver, lit);
		}
		ipasir_add(solver,0);
	}
	fin.close();
}

bool check_res(string name) {
	ifstream fin(name.c_str());
	string line;
	while(getline(fin,line)){
		if(line.size()==0) continue;
		if(line[0] == 'p' || line[0] == 'c') continue;
		stringstream ss(line);
		int lit, flag = 0;
		while(ss>>lit){
			if(lit==0) break;
			if (lit > 0 && val[lit] > 0) flag = 1;
			if (lit < 0 && val[-lit] < 0) flag = 1;
		}
		if (!flag) return false;
	}
	fin.close();
	return true;
}

void test1(){
	printf("-----------------------------------test1------------------------------\n");
	int res = 0;

	void * solver = ipasir_init();
	load_bones(solver,string("sat250.cnf"));
	cout<<"c res="<<ipasir_solve(solver)<<endl;
	cout<<"c res="<<ipasir_solve(solver)<<endl;
	ipasir_release(solver);

	void * solver2 = ipasir_init();
	load_bones(solver2,string("dlx1c.ucl.sat.chaff.4.1.bryant.cnf"));
	cout<<"c res="<<ipasir_solve(solver2)<<endl;
	ipasir_release(solver2);
	
	void * solver3 = ipasir_init();
	load_bones(solver3,string("sat250.cnf"));
	cout<<"c res="<<ipasir_solve(solver3)<<endl;
	ipasir_release(solver3);
	
	void * solver4 = ipasir_init();
	load_bones(solver4,string("dlx1c.ucl.sat.chaff.4.1.bryant.cnf"));
	cout<<"c res="<<ipasir_solve(solver4)<<endl;
	cout<<"c res="<<ipasir_solve(solver4)<<endl;
	ipasir_release(solver4);
	printf("-----------------------------------done------------------------------\n");
}

void test2() {
	printf("-----------------------------------test2------------------------------\n");
	void * solver5 = ipasir_init();
	ifstream fin("dlx1c.ucl.sat.chaff.4.1.bryant.cnf");
	string line;
	int ll = 0;
	while(getline(fin,line)){
		ll++;
		if(line.size()==0) continue;
		if(line[0] == 'p' || line[0] == 'c') continue;
		stringstream ss(line);
		int lit;
		while(ss>>lit){
			if(lit==0) break;
			ipasir_add(solver5, lit);
		}
		ipasir_add(solver5,0);
		if (ll >= 43300) cout<<"c res="<<ipasir_solve(solver5)<< ": time " << ll << endl;
	}
	cout<<"c res="<<ipasir_solve(solver5)<< ": time " << endl;
	fin.close();
	ipasir_release(solver5);
	printf("-----------------------------------done------------------------------\n");
}

void test3() {
	printf("-----------------------------------test3------------------------------\n");
	void * solver = ipasir_init();
	load_bones(solver,string("sat250.cnf"));
	cout<<"c res="<<ipasir_solve(solver)<<endl;
	for (int i = 1; i <= 250; i++)
		val[i] = ipasir_val(solver, i), printf("%d ", val[i]);
	puts("");
	for (int i = 1; i <= 250; i++) {
		ipasir_assume(solver, -val[i]);
		cout<<"c time: " << i << "  res="<<ipasir_solve(solver)<<endl;
	}
	ipasir_release(solver);
}

// void test3() {
	
// 	for (int i = 1; i <= 250; i++)
// 		ipasir_assume(solver, -val[i]);
// 	cout<<"c res="<<ipasir_solve(solver)<<endl;
	
// 	for (int i = 1; i <= 250; i++)
// 		val[i] = ipasir_val(solver, i), printf("%d ", val[i]);
// 	puts("");
// 	flag = check_res("sat250.cnf");
// 	if (flag) printf("Right!!\n\n");
// 	else printf("Wrong\n\n");


// 	for(int i=10;i<60;i+=10){
// 		cout<<"\nc check "<<i<<endl;
// 		ipasir_assume(solver,i);
// 		cout<<"c res="<<ipasir_solve(solver)<<endl;

// 		cout<<"\nc check bones: "<<-i<<endl;
// 		ipasir_assume(solver,-i);
// 		cout<<"c res="<<ipasir_solve(solver)<<endl;
// 	}
// }
int main(){
	test3();
	//test2();
	// check_bones();
	return 0;
}
#include "faninselection_c.h"
#include <csetjmp>
#include <csignal>
#include <ctime>
#include <signal.h>
#include <setjmp.h>

using namespace std;

sigjmp_buf env;

void timeout_act(int) {
	siglongjmp(env, 1);
}

FaninSelectorC::FaninSelectorC(Circuit * spec_circuit, Circuit * impl_circuit, NodeVector * targets, NodeVector * candidates, NodeVector * solution_nodes, int iteration_limit)
{
	NodeVector empty;
	this->targets = targets;
	this->cands = candidates;
	this->inputs = &(impl_circuit->inputs);
	this->outputs = &(impl_circuit->outputs);
	impl.Init("impl", impl_circuit, targets, candidates);
	spec.Init("spec", spec_circuit, &empty, &empty);
	MakeSolver();
	std::cout << "A solver has been made up" << std::endl;
}

bool FaninSelectorC::MakeSolver()
{
	num_t = targets->size();
	num_cnf = pow(2, num_t);

	//construct the CNF of impl
	for (int i = 0; i < 2 * num_cnf; i++)
	{
		main.addCNF(&impl, impl.numVars() * i);
		int tmp = i;
		for (int j = 1; j < num_t; j++) {
			int val = tmp % 2;
			tmp /= 2;
			main.addClause(impl.GetIndex(targets->at(i))+ i * impl.numVars() , bool(val));
		}
	}

	main.addCNF(&spec, impl.numVars() * 2 * num_cnf);
	main.addCNF(&spec, impl.numVars() * 2 * num_cnf + spec.numVars());

	//inputs
	for (auto pi : *inputs) {
		int in_impl = impl.GetIndex(pi->name);
		int in_spec = spec.GetIndex(pi->name);
		for (int i = 0; i < num_cnf; i++) {
			main.setEqual(in_impl, in_impl + i * impl.numVars());
			main.setEqual(in_impl + num_cnf * impl.numVars(), in_impl + (num_cnf + i) * impl.numVars());

		}
		main.setEqual(in_impl, in_spec +  2 * num_cnf * impl.numVars());
		main.setEqual(in_impl + num_cnf * impl.numVars(), in_spec + 2 * num_cnf * impl.numVars() + spec.numVars());
	}
	//outputs
	for (size_t i = 0; i < num_cnf; i++){
		std::vector<int> out_vec_impl, out_vec_spec;
		for (auto po : *outputs) {
			int out_impl = impl.GetIndex(po->name);
			int out_spec = spec.GetIndex(po->name);
			out_vec_impl.push_back(out_impl + i * impl.numVars());
			out_vec_impl.push_back(out_impl + (num_cnf + i) * impl.numVars());
			out_vec_spec.push_back(out_spec + 2 * num_cnf * impl.numVars());
			out_vec_spec.push_back(out_spec + 2 * num_cnf * impl.numVars() + spec.numVars());
		}
		main.setEqual(out_vec_impl, out_vec_spec);
	}
	
}

bool FaninSelectorC::solve() {
	SC::setcover* set = new SC::setcover(*cands);
	vector<bool> row;
	row.reserve(cands->size());
	vector<vector<bool>> rows;
	int times = 1;

	cout << "-------------------iteration starts-----------------" << endl;

	while (1){
		cout << "iteration:" << times << endl;
		Glucose::vec<Glucose::Lit> con;

		struct sigaction s_action;
		struct itimerval timer, oldtimer;
		timer.it_value.tv_sec = 300;
		timer.it_value.tv_usec = 0;
		timer.it_interval.tv_sec = 300;
		timer.it_interval.tv_usec = 0;
		memset(&s_action, 0, sizeof(s_action));
		s_action.sa_handler = timeout_act;
		sigaction(SIGALRM, &s_action, NULL);
		sigset_t block;
		//sigemptyset(&block); 
		sigaddset(&block, SIGALRM);
		sigprocmask(SIG_UNBLOCK, &block, NULL);
		setitimer(ITIMER_REAL, &timer, &oldtimer);
		bool res = false;
		if (!sigsetjmp(env, 0))
			res = main.solver->solve(con);
		else {
			main.solver->interrupt();
			std::cout << "TIME OUT" << std::endl;
		}
		setitimer(ITIMER_REAL, &oldtimer, NULL);
		if (res == true) {
			cout << "SATISFIABLE" << endl;
			map<Node *, bool> incases;

			cout << "New pair of cases found" << endl;
			for (auto cand : *cands) {
				incases[cand] = (main.modelNum(impl.GetIndex(cand->name) + num_cnf * impl.numVars()) == l__True);
			}
		} 
	}
}
#include <iostream>
#include <csetjmp>
#include <csignal>
#include <ctime>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>
#include "mergeselection.h"
#include "getDC.h"

using namespace std;

sigjmp_buf env;

void timeout_act_m(int) {
	siglongjmp(env, 1);
}

MergeSelection::MergeSelection(Circuit* impl_circuit, Circuit* spec_circuit, Cluster* seed, ClusterVector* candidates, NodeVector* candidate_inputs, int limit) {
	if (seed->nodes_c.size() == 1) {
		this->seed = seed->nodes_c.at(0);
		this->cands_c = *candidates;
		this->cand_inputs = *candidate_inputs;
		this->impl_circuit = impl_circuit;
		this->limit = limit;
		nodecircuit::NodeVector empty;
		nodecircuit::NodeVector targets;
		targets.push_back(seed->nodes_c.at(0));
		impl.Init("impl", impl_circuit, &targets, &cand_inputs);
		spec.Init("spec", spec_circuit, &empty, &empty);
		MakeSolver();
		Solve_c();

		
	}
	else
		cout << "the function for multiple targets is not available yet" << endl;
}

//MergeSelection::MergeSelection(Circuit* impl_circuit, Circuit* spec_circuit, Node* seed, NodeVector* candidates, NodeVector* candidate_inputs) {
//	this->seed = seed;
//	this->cands = *candidates;
//	this->cand_inputs = *candidate_inputs;
//	this->impl_circuit = impl_circuit;
//	nodecircuit::NodeVector empty;
//	nodecircuit::NodeVector targets;
//	targets.push_back(seed);
//	impl.Init("impl", impl_circuit, &targets, &cand_inputs);
//	spec.Init("spec", spec_circuit, &empty, &empty);
//	MakeSolver();
//	Solve();
//
//	//GetOnSet(impl_circuit);
//}

void MergeSelection::MakeSolver() {
	//impl,spec
	for (int i = 0; i < 4; i++) {
		glu.addCNF(&impl, impl.numVars()*i);
	}
	glu.addCNF(&spec, impl.numVars() * 4);
	glu.addCNF(&spec, impl.numVars() * 4 + spec.numVars());
	//impl target
	for (int i = 0; i < 4; i++) {
		int tmp = i % 2;
		glu.solver->addClause(glu.genLit(impl.GetIndex(seed->name) + impl.numVars()*i, (bool)(tmp!=0)));
	}
	//inputs
	for (auto pi : impl_circuit->inputs) {
		glu.setEqual(impl.GetIndex(pi->name), spec.GetIndex(pi->name) + impl.numVars() * 4);
		glu.setEqual(impl.GetIndex(pi->name) + impl.numVars(), spec.GetIndex(pi->name) + impl.numVars() * 4);
		glu.setEqual(impl.GetIndex(pi->name) + impl.numVars() * 2, spec.GetIndex(pi->name) + impl.numVars() * 4 + spec.numVars());
		glu.setEqual(impl.GetIndex(pi->name) + impl.numVars() * 3, spec.GetIndex(pi->name) + impl.numVars() * 4 + spec.numVars());
	}
	//outputs
	vector<int> out_spec0, out_spec1, out_impl0F, out_impl0T, out_impl1F, out_impl1T;
	for (auto po : impl_circuit->outputs) {
		out_impl0F.push_back(impl.GetIndex(po->name));
		out_impl0T.push_back(impl.GetIndex(po->name) + impl.numVars());
		out_impl1F.push_back(impl.GetIndex(po->name) + impl.numVars() * 2);
		out_impl1T.push_back(impl.GetIndex(po->name) + impl.numVars() * 3);
		out_spec0.push_back(spec.GetIndex(po->name) + impl.numVars() * 4);
		out_spec1.push_back(spec.GetIndex(po->name) + impl.numVars() * 4 + spec.numVars());
	}
	glu.setEqual(out_impl0F, out_spec0);
	glu.setEqual(out_impl1T, out_spec1);
	glu.setNEqual(out_impl0T, out_spec0);
	glu.setNEqual(out_impl1F, out_spec1);

	en_in0 = glu.solver->nVars();
	en_in1 = glu.solver->nVars() + 1;
	glu.genLit(en_in0, true);
	glu.genLit(en_in1, true);

	for (auto cand : cand_inputs) {
		std::vector<int> sig1, sig2;
		sig1 = { impl.GetIndex(cand->name) };
		sig2 = { impl.GetIndex(cand->name) + impl.numVars() * 2 };
		int eq = glu.solver->nVars();
		glu.ifTrue_thenEQ(sig1, sig2, eq);
		eq_trigger[cand] = eq;
	}

	cout << "a solver is made" << endl;
}

void MergeSelection::removeCase(std::map<nodecircuit::Node *, bool> data, FType type)
{
	Glucose::vec<Glucose::Lit> lits;
	if (type == IN0)
	{
		for (auto sig : cand_inputs)
		{
			lits.push(glu.genLit(impl.GetIndex(sig->name), !data[sig]));
			lits.push(glu.genLit(en_in0, true));
		}
	}
	else
	{
		for (auto sig : cand_inputs)
		{
			lits.push(glu.genLit(impl.GetIndex(sig->name) + 2 * impl.numVars(), !data[sig]));
			lits.push(glu.genLit(en_in1, true));
		}
	}
	glu.solver->addClause(lits);
}

void MergeSelection::Solve_c() {
	bool no_in0case = false;
	bool no_in1case = false;
	bool flag; // show
	bool first_iteration = true;
	set = new setcover_m(cands_c, cand_inputs);
	std::vector<bool> row;
	row.reserve(cand_inputs.size());
	std::vector<std::vector<bool>> rows;
	nodecircuit::NodeVector empty;
	int times = 1;

	std::cout << "---------------iteration starts---------------" << std::endl;

	//while (times < iteration_limit)
	while (1)
	{
		std::cout << "iteration: " << times << std::endl;
		Glucose::vec<Glucose::Lit> con;
		//solution = { impl_circuit->GetNode("n3844"), impl_circuit->GetNode("n3840"), impl_circuit->GetNode("n7422"), impl_circuit->GetNode("n7060"),impl_circuit->GetNode("n10168") };//debug
		//solution = { impl_circuit->GetNode("n3840"), impl_circuit->GetNode("n7422") };//debug
		//solution = { impl_circuit->GetNode("n1174_1"), impl_circuit->GetNode("n1194_1"), impl_circuit->GetNode("n1193"), impl_circuit->GetNode("n1195") };
		for (auto sig : solution_inputs)
			con.push(glu.genLit(eq_trigger[sig], true));
		con.push(glu.genLit(en_in0, no_in0case));
		con.push(glu.genLit(en_in1, no_in1case));
		struct sigaction s_action;
		struct itimerval timer, oldtimer;
		timer.it_value.tv_sec = 10;
		timer.it_value.tv_usec = 0;
		timer.it_interval.tv_sec = 10;
		timer.it_interval.tv_usec = 0;
		memset(&s_action, 0, sizeof(s_action));
		s_action.sa_handler = timeout_act_m;
		sigaction(SIGALRM, &s_action, NULL);
		sigset_t block;
		//sigemptyset(&block); 
		sigaddset(&block, SIGALRM);
		sigprocmask(SIG_UNBLOCK, &block, NULL);
		setitimer(ITIMER_REAL, &timer, &oldtimer);
		bool res = false;
		if (!sigsetjmp(env, 0))
			res = glu.solver->solve(con);
		else {
			std::cout << "TIME OUT" << std::endl;
			//glu.solver->interrupt();
			glu = Glucose::GInterface();
			MakeSolver();
			for (auto in0case : in0cases)
				removeCase(in0case, IN0);
			for (auto in1case : in1cases)
				removeCase(in1case, IN1);
		}
		setitimer(ITIMER_REAL, &oldtimer, NULL);
		if (res == true)
		{
			flag = false;
			first_iteration = false;
			std::cout << "SATISFIABLE" << std::endl;
			std::map<nodecircuit::Node *, bool> in0case;
			std::map<nodecircuit::Node *, bool> in1case;

			if (!no_in0case)
			{
				std::cout << "New in0case found" << std::endl;
				for (auto sig : cand_inputs)
				{
					in0case[sig] = (glu.modelNum(impl.GetIndex(sig->name)) == l__True);
				}
				for (auto in1 : in1cases)
				{
					for (auto sig : cand_inputs)
					{
						row.push_back((in0case[sig] != in1[sig]) ? true : false);
					}
					rows.push_back(row);
					row.clear();
				}
				in0cases.push_back(in0case);
				removeCase(in0case, IN0);
			}

			if (!no_in1case)
			{
				std::cout << "New in1case found" << std::endl;
				for (auto sig : cand_inputs)
				{
					in1case[sig] = (glu.modelNum(impl.GetIndex(sig->name) + impl.numVars() * 2) == l__True);
				}
				for (auto in0 : in0cases)
				{
					for (auto sig : cand_inputs)
					{
						row.push_back((in1case[sig] != in0[sig]) ? true : false);
					}
					rows.push_back(row);
					//for (auto lit : row)
					//std::cout << lit;
					//std::cout << std::endl;
					row.clear();
				}
				in1cases.push_back(in1case);
				removeCase(in1case, IN1);
			}
		}
		else
		{
			std::cout << "UNSATISFIABLE" << std::endl;
			if (first_iteration) {
				//constant_zero = in0.solver->solve();
				//constant_one = in1.solver->solve;
				break;
			}
			if (flag == true)
			{
				std::cout << "Solution is found" << std::endl;
				delete set;
				break;
			}
			if (!no_in1case)
			{
				if (!no_in0case)
				{
					std::cout << "No more in0case or in1case." << std::endl;
					std::cout << "Asumming no more in1case" << std::endl;
					no_in1case = true;
					continue;
				}
				no_in1case = true;
				no_in0case = false;
				flag = true;
				std::cout << "No more in1case" << std::endl;
				continue;
			}
			else if (!no_in0case)
			{
				no_in0case = true;
				no_in1case = false;
				flag = true;
				std::cout << "No more in0case" << std::endl;
				continue;
			}
		}

		//for (vector<bool> row_ : rows) {
		//	for (bool value : row_) {
		//		cout << value;
		//	}
		//	cout << endl;
		//}
		//setcovering
		times++;
		solution_c.clear();
		solution_inputs.clear();
		//if (times > iteration_limit)
		//	break;
		set->AddRows(rows);
		rows.clear();
		set->Select(solution_c, solution_inputs, limit);
		if (solution_c.size() != 0)
			std::cout << "set covering finished!\n\n\n" << std::endl;
		else
			break;
	}
	cout << "\n****************************\n";
	cout << "selected cluster: ";
	for (Cluster* clst : solution_c) {
		cout << clst->name << "(";
		for (Node* in : clst->input_nodes) {
			cout << in->name << ", ";
		}
		cout << "), ";
	}
	cout << endl;
	cout << "selected nodes' inputs: ";
	for (Node* node : solution_inputs) {
		cout << node->name << ", ";
	}
	cout << "\noriginal nodes' inputs: ";
	for (Node* node : seed->inputs_pre) {
		cout << node->name << ", ";
	}
	cout << "\nnum of inputs changed from " << seed->inputs_pre.size() << " to " << solution_inputs.size();
	cout << "\n***************************";
	cout << endl;

};

void MergeSelection::GetOnSet() {
	NodeVector solution_inputs_vec;
	solution_inputs_vec.insert(solution_inputs_vec.begin(),solution_inputs.begin(), solution_inputs.end());
	cout << "getdc" << endl;
	DC::getDC dc(&solution_inputs_vec, seed , &impl, &spec, impl_circuit);
	cout << "solve" << endl;
	dc.solve();
	dc.dumpCases();
	onset = dc.in2cases;
};



//void MergeSelection::Solve() {
//	bool no_in0case = false;
//	bool no_in1case = false;
//	bool flag; // show
//	bool first_iteration = true;
//	set = new setcover_m(cands, cand_inputs);
//	std::vector<bool> row;
//	row.reserve(cand_inputs.size());
//	std::vector<std::vector<bool>> rows;
//	nodecircuit::NodeVector empty;
//	int times = 1;
//
//	std::cout << "---------------iteration starts---------------" << std::endl;
//
//	//while (times < iteration_limit)
//	while (1)
//	{
//		std::cout << "iteration: " << times << std::endl;
//		Glucose::vec<Glucose::Lit> con;
//		//solution = { impl_circuit->GetNode("n3844"), impl_circuit->GetNode("n3840"), impl_circuit->GetNode("n7422"), impl_circuit->GetNode("n7060"),impl_circuit->GetNode("n10168") };//debug
//		//solution = { impl_circuit->GetNode("n3840"), impl_circuit->GetNode("n7422") };//debug
//		//solution = { impl_circuit->GetNode("n1174_1"), impl_circuit->GetNode("n1194_1"), impl_circuit->GetNode("n1193"), impl_circuit->GetNode("n1195") };
//		for (auto sig : solution_inputs)
//			con.push(glu.genLit(eq_trigger[sig], true));
//		con.push(glu.genLit(en_in0, no_in0case));
//		con.push(glu.genLit(en_in1, no_in1case));
//		struct sigaction s_action;
//		struct itimerval timer, oldtimer;
//		timer.it_value.tv_sec = 300;
//		timer.it_value.tv_usec = 0;
//		timer.it_interval.tv_sec = 300;
//		timer.it_interval.tv_usec = 0;
//		memset(&s_action, 0, sizeof(s_action));
//		s_action.sa_handler = timeout_act_m;
//		sigaction(SIGALRM, &s_action, NULL);
//		sigset_t block;
//		//sigemptyset(&block); 
//		sigaddset(&block, SIGALRM);
//		sigprocmask(SIG_UNBLOCK, &block, NULL);
//		setitimer(ITIMER_REAL, &timer, &oldtimer);
//		bool res = false;
//		if (!sigsetjmp(env, 0))
//			res = glu.solver->solve(con);
//		else {
//			glu.solver->interrupt();
//			std::cout << "TIME OUT" << std::endl;
//		}
//		setitimer(ITIMER_REAL, &oldtimer, NULL);
//		if (res == true)
//		{
//			flag = false;
//			first_iteration = false;
//			std::cout << "SATISFIABLE" << std::endl;
//			std::map<nodecircuit::Node *, bool> in0case;
//			std::map<nodecircuit::Node *, bool> in1case;
//
//			if (!no_in0case)
//			{
//				std::cout << "New in0case found" << std::endl;
//				for (auto sig : cand_inputs)
//				{
//					in0case[sig] = (glu.modelNum(impl.GetIndex(sig->name)) == l__True);
//				}
//				for (auto in1 : in1cases)
//				{
//					for (auto sig : cand_inputs)
//					{
//						row.push_back((in0case[sig] != in1[sig]) ? true : false);
//					}
//					rows.push_back(row);
//					row.clear();
//				}
//				in0cases.push_back(in0case);
//				removeCase(in0case, IN0);
//			}
//
//			if (!no_in1case)
//			{
//				std::cout << "New in1case found" << std::endl;
//				for (auto sig : cand_inputs)
//				{
//					in1case[sig] = (glu.modelNum(impl.GetIndex(sig->name) + impl.numVars() * 2) == l__True);
//				}
//				for (auto in0 : in0cases)
//				{
//					for (auto sig : cand_inputs)
//					{
//						row.push_back((in1case[sig] != in0[sig]) ? true : false);
//					}
//					rows.push_back(row);
//					//for (auto lit : row)
//						//std::cout << lit;
//					//std::cout << std::endl;
//					row.clear();
//				}
//				in1cases.push_back(in1case);
//				removeCase(in1case, IN1);
//			}
//		}
//		else
//		{
//			std::cout << "UNSATISFIABLE" << std::endl;
//			if (first_iteration) {
//				//constant_zero = in0.solver->solve();
//				//constant_one = in1.solver->solve;
//				break;
//			}
//			if (flag == true)
//			{
//				std::cout << "Solution is found" << std::endl;
//				delete set;
//				break;
//			}
//			if (!no_in1case)
//			{
//				if (!no_in0case)
//				{
//					std::cout << "No more in0case or in1case." << std::endl;
//					std::cout << "Asumming no more in1case" << std::endl;
//					no_in1case = true;
//					continue;
//				}
//				no_in1case = true;
//				no_in0case = false;
//				flag = true;
//				std::cout << "No more in1case" << std::endl;
//			}
//			else if (!no_in0case)
//			{
//				no_in0case = true;
//				no_in1case = false;
//				flag = true;
//				std::cout << "No more in0case" << std::endl;
//			}
//		}
//
//		//for (vector<bool> row_ : rows) {
//		//	for (bool value : row_) {
//		//		cout << value;
//		//	}
//		//	cout << endl;
//		//}
//		//setcovering
//		times++;
//		solution.clear();
//		solution_inputs.clear();
//		//if (times > iteration_limit)
//		//	break;
//		set->AddRows(rows);
//		rows.clear();
//		set->Select(solution, solution_inputs);
//		if (solution.size() != 0)
//			std::cout << "set covering finished!\n\n\n" << std::endl;
//		else
//			break;
//	}
//	cout << "\n****************************\n";
//	cout << "selected nodes: ";
//	for (Node* node : solution) {
//		cout << node->name << "(";
//		for (Node* in : node->inputs) {
//			cout << in->name << ", ";
//		}
//		cout << "), ";
//	}
//	cout << endl;
//	cout << "selected nodes' inputs: ";
//	for (Node* node : solution_inputs) {
//		cout << node->name << ", ";
//	}
//	cout << "\noriginal nodes' inputs: ";
//	for (Node* node : seed->inputs_pre) {
//		cout << node->name << ", ";
//	}
//	cout << "\nnum of inputs changed from " << seed->inputs_pre.size() << " to " << solution_inputs.size();
//	cout << "\n***************************";
//	cout << endl;
//
//};
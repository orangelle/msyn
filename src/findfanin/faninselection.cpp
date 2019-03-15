#include "faninselection.h"
#include <csetjmp>
#include <csignal>
#include <ctime>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>

namespace fanin_selection {
	sigjmp_buf env;

	void timeout_act(int) {
		siglongjmp(env, 1);
	}
	FaninSelector::FaninSelector(nodecircuit::Circuit *impl_circuit,
		nodecircuit::Circuit *spec_circuit,
		nodecircuit::NodeVector *candidates,
		nodecircuit::NodeVector *targets, int iteration_limit)
	{
		nodecircuit::NodeVector empty;
		this->candidates = candidates;
		this->targets = targets;
		this->iteration_limit = iteration_limit;
		this->impl_circuit = impl_circuit;
		impl.Init("impl", impl_circuit, targets, candidates);
		spec.Init("spec", spec_circuit, &empty, &empty);
		if (!makeSolver())
			std::cout << "Failed to make up a solver" << std::endl;
		else {
			std::cout << "A solver has been made up" << std::endl;
			this->solve();
		}
	}

	bool FaninSelector::makeSolver()
	{
		int t_num = targets->size();

		// construct a solver
		// add the CNF of impl
		for (int i = 0; i < (2 + 2 * pow(2, t_num - 1)); i++)
		{
			main.addCNF(&impl, impl.numVars() * i);
			//std::cout << "add CNF of impl" << i << std::endl;
		}

		// add CNF of spec
		main.addCNF(&spec, impl.numVars() * (2 + 2 * pow(2, t_num - 1)));
		main.addCNF(&spec, impl.numVars() * (2 + 2 * pow(2, t_num - 1)) + spec.numVars());
		//std::cout << "add CNF of spec" << std::endl;

		// add targets
		main.solver->addClause(main.genLit(impl.GetIndex(targets->at(0)), false)); // in0, target0 =  0
		//std::cout << "set target 0 = 0 for impl 0" << std::endl;
		main.solver->addClause(main.genLit(impl.GetIndex(targets->at(0)) + impl.numVars(), true)); // in1, target0 = 1
		//std::cout << "set target 0 = 1 for impl 1" << std::endl;

		int n_impl = 1;
		for (int i = 0; i < pow(2, t_num - 1); i++)
		{
			n_impl++;
			main.solver->addClause(main.genLit(impl.GetIndex(targets->at(0)) + n_impl * impl.numVars(), true)); // in0, target0 = 1
			//std::cout << "set target 0 = 1 for impl "<< n_impl << std::endl;
			int temp = i;
			for (int j = 1; j < t_num; j++)
			{
				int val = temp % 2;
				val == 0
					? main.solver->addClause(main.genLit(impl.GetIndex(targets->at(j)) + n_impl * impl.numVars(), false))
					: main.solver->addClause(main.genLit(impl.GetIndex(targets->at(j)) + n_impl * impl.numVars(), true));
				temp = temp / 2;
				//std::cout << "set target "<< j << " = " << val << " for impl " << n_impl << std::endl;
			}
		}

		for (int i = 0; i < pow(2, t_num - 1); i++)
		{
			n_impl++;
			main.solver->addClause(main.genLit(impl.GetIndex(targets->at(0)) + n_impl * impl.numVars(), false)); // in1, target0 = 0
			//std::cout << "set target 0 = 0 for impl " << n_impl << std::endl;
			int temp = i;
			for (int j = 1; j < t_num; j++)
			{
				int val = temp % 2;
				val == 0
					? main.solver->addClause(main.genLit(impl.GetIndex(targets->at(j)) + n_impl * impl.numVars(), false))
					: main.solver->addClause(main.genLit(impl.GetIndex(targets->at(j)) + n_impl * impl.numVars(), true));
				temp = temp / 2;
				//	std::cout << "set target " << j << " = " << val << " for impl " << n_impl << std::endl;
			}
		}

		// set inputs

		for (auto pin : impl_circuit->inputs)
		{
			n_impl = 1;
			int in_impl = impl.GetIndex(pin->name);
			int in_spec = spec.GetIndex(pin->name);
			// in0
			for (int i = 0; i < pow(2, t_num - 1); i++)
			{
				n_impl++;
				main.setEqual(in_impl, in_impl + n_impl * impl.numVars());
				//std::cout << "make " << pin->name <<" same between impl 0 and impl " << n_impl << std::endl;
			}
			main.setEqual(in_impl, in_spec + impl.numVars() * (2 + 2 * pow(2, t_num - 1)));
			//std::cout << "make " << pin->name << " same between impl 0 and spec 0" << std::endl;
			// in1
			for (int i = 0; i < pow(2, t_num - 1); i++)
			{
				n_impl++;
				main.setEqual(in_impl + impl.numVars(), in_impl + n_impl * impl.numVars());
				//std::cout << "make " << pin->name << " same between impl 1 and impl " << n_impl << std::endl;
			}
			main.setEqual(in_impl + impl.numVars(), in_spec + impl.numVars() * (2 + 2 * pow(2, t_num - 1)) + spec.numVars());
			//std::cout << "make " << pin->name << " same between impl 1 and spec 1" << std::endl;
		}

		// set outputs
		std::vector<int> out_in0_0, out_in1_1, out_in0_s, out_in1_s;
		for (auto pout : impl_circuit->outputs)
		{
			int out_impl = impl.GetIndex(pout->name);
			int out_spec = spec.GetIndex(pout->name);
			out_in0_0.push_back(out_impl);
			out_in1_1.push_back(out_impl + impl.numVars());
			out_in0_s.push_back(out_spec + impl.numVars() * (2 + 2 * pow(2, t_num - 1)));
			out_in1_s.push_back(out_spec + impl.numVars() * (2 + 2 * pow(2, t_num - 1)) + spec.numVars());
		}
		main.setEqual(out_in0_s, out_in0_0);
		main.setEqual(out_in1_s, out_in1_1);
		n_impl = 1;
		for (int i = 0; i < pow(2, t_num - 1); i++)
		{
			n_impl++;
			std::vector<int> out_in0_1, out_in1_0;
			for (auto pout : impl_circuit->outputs)
			{
				int out_impl = impl.GetIndex(pout->name);
				out_in0_1.push_back(out_impl + n_impl * impl.numVars());
				out_in1_0.push_back(out_impl + (n_impl + pow(2, t_num - 1)) * impl.numVars());
			}
			main.setNEqual(out_in0_1, out_in0_s);
			main.setNEqual(out_in1_0, out_in1_s);
		}
		en_in0 = main.solver->nVars();
		en_in1 = main.solver->nVars() + 1;
		main.genLit(en_in0, true);
		main.genLit(en_in1, true);

		// add constraint
		for (auto sig : *candidates)
		{
			std::vector<int> sig1, sig2;
			sig1 = { impl.GetIndex(sig->name) };
			sig2 = { impl.GetIndex(sig->name) + impl.numVars() };
			int eq = main.solver->nVars();
			main.ifTrue_thenEQ(sig1, sig2, eq);
			eq_trigger[sig] = eq;
		}

		return 1;
	}

	bool FaninSelector::solve()
	{
		bool no_in0case = false;
		bool no_in1case = false;
		bool flag; // show
		bool first_iteration = true;
		set = new SC::setcover(*candidates);
		std::vector<bool> row;
		row.reserve(candidates->size());
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
			for (auto sig : solution)
				con.push(main.genLit(eq_trigger[sig], true));
			con.push(main.genLit(en_in0, no_in0case));
			con.push(main.genLit(en_in1, no_in1case));
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
					for (auto sig : *candidates)
					{
						in0case[sig] = (main.modelNum(impl.GetIndex(sig->name)) == l__True);
					}
					for (auto in1 : in1cases)
					{
						for (auto sig : *candidates)
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
					for (auto sig : *candidates)
					{
						in1case[sig] = (main.modelNum(impl.GetIndex(sig->name) + impl.numVars()) == l__True);
					}
					for (auto in0 : in0cases)
					{
						for (auto sig : *candidates)
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
				}
				else if (!no_in0case)
				{
					no_in0case = true;
					no_in1case = false;
					flag = true;
					std::cout << "No more in0case" << std::endl;
				}
			}

			//setcovering
			times++;
			solution.clear();
			if (times > iteration_limit)
				break;
			set->AddRows(rows);
			rows.clear();
			set->SolveMinCol(solution, empty, empty);
			assert(solution.size() != 0);
			std::cout << "Set covering finished!" << std::endl;
		}
		return 0;
	}

	void FaninSelector::removeCase(std::map<nodecircuit::Node *, bool> data, FType type)
	{
		Glucose::vec<Glucose::Lit> lits;
		if (type == IN0)
		{
			for (auto sig : *candidates)
			{
				lits.push(main.genLit(impl.GetIndex(sig->name), !data[sig]));
				lits.push(main.genLit(en_in0, true));
			}
		}
		else
		{
			for (auto sig : *candidates)
			{
				lits.push(main.genLit(impl.GetIndex(sig->name) + impl.numVars(), !data[sig]));
				lits.push(main.genLit(en_in1, true));
			}
		}
		main.solver->addClause(lits);
	}
}

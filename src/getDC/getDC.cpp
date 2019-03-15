#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "utils/System.h"
#include "core/Dimacs.h"
#include "simp/SimpSolver.h"
#include "getDC.h"
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>

using namespace DC;

sigjmp_buf env_sigalrm_dc;

void getDC::Timeout(int signum) {
  std::cout << "Timeout" << std::endl;
  siglongjmp(env_sigalrm_dc, 1);
}

getDC::getDC(nodecircuit::NodeVector *arg_signals, nodecircuit::NodeVector *arg_targets, CNFgen::CNFGenerator *arg_fcnf, CNFgen::CNFGenerator *arg_gcnf, nodecircuit::Circuit *arg_impl) :
  circuit(arg_impl), signals(arg_signals),targets(arg_targets),fcnf(arg_fcnf),gcnf(arg_gcnf)
{
  //add CNF of impl
  int t_num = targets->size();
  for(int i = 0; i < pow(2, t_num - 1); i++)
  {
    main.addCNF(fcnf, fcnf->numVars() * i);
  }
  //add CNF of spec
  main.addCNF(gcnf, fcnf->numVars() * pow(2, t_num -1));

  //add targets
	for (int i = 0; i < pow(2, t_num - 1); i++)
	{
		main.solver->addClause(main.genLit(fcnf->GetIndex(targets->at(0)) + i * fcnf->numVars(), false)); // in1, target0 = 0;
		int temp = i;
		for (int j = 1; j < t_num; j++)
		{
			temp % 2 == 0 
				? main.solver->addClause(main.genLit(fcnf->GetIndex(targets->at(j)) + i * fcnf->numVars(), false)) 
				: main.solver->addClause(main.genLit(fcnf->GetIndex(targets->at(j)) + i* fcnf->numVars(), true));
			temp = temp / 2;
		}
	}
	
	//add inputs
	for (auto pin : circuit->inputs)
	{
		int in_impl = fcnf->GetIndex(pin);
		int in_spec = gcnf->GetIndex(pin);
		for (int i = 1; i < pow(2, t_num - 1); i++)
		{
			main.setEqual(in_impl, in_impl + fcnf->numVars() * i);
		}
		main.setEqual(in_impl, in_spec + fcnf->numVars() * pow(2, t_num - 1));
	}

	//add outputs
	std::vector<int> out_s;
		for (auto pout : circuit->outputs)
		{
			out_s.push_back(gcnf->GetIndex(pout) + fcnf->numVars() * pow(2, t_num - 1));
		}

	for (int i = 0; i < pow(2, t_num - 1); i++)
	{
		std::vector<int> out_i;
		for (auto pout : circuit->outputs)
		{
			out_i.push_back(fcnf->GetIndex(pout) + fcnf->numVars() * i);
		}
		main.setNEqual(out_i, out_s);
	}
}

void getDC::solve_multi()
{
	while (1){
		struct itimerval timer, oldtimer;
		struct sigaction s_action;
		timer.it_value.tv_sec = 60;
		timer.it_interval.tv_sec = 60;
		timer.it_value.tv_usec = 0;
		timer.it_interval.tv_usec = 0;
		memset(&s_action, 0, sizeof(s_action));
		s_action.sa_handler = Timeout;
		sigaction(SIGALRM, &s_action, NULL);
		sigset_t block;
		sigaddset(&block, SIGALRM);
		sigprocmask(SIG_UNBLOCK, &block, NULL);
		setitimer(ITIMER_REAL, &timer, &oldtimer);
		if (!sigsetjmp(env_sigalrm_dc, 0)) {
			if (main.getAnswer(false) == false) {
				setitimer(ITIMER_REAL, &oldtimer, NULL);
				break;
			}
			else {
				std::map<nodecircuit::Node*, bool> tmp;
				for (auto sig : *signals) {
					tmp[sig] = (main.modelNum(fcnf->GetIndex(sig)) == l__True);
				}
				removeCase(&tmp, IN2);
				//for (auto pair : tmp) {
				//	std::cout << pair.first->name << "(" << pair.second << ")";
				//}
				//std::cout << std::endl;
			}
		}
		else {
			break;
			setitimer(ITIMER_REAL, &oldtimer, NULL);
		}
		setitimer(ITIMER_REAL, &oldtimer, NULL);
	}
}

getDC::getDC(nodecircuit::NodeVector *arg_signals, nodecircuit::Node *arg_target, CNFgen::CNFGenerator *arg_fcnf, CNFgen::CNFGenerator *arg_gcnf, nodecircuit::Circuit *arg_impl) :
  circuit(arg_impl), signals(arg_signals),target(arg_target),fcnf(arg_fcnf),gcnf(arg_gcnf)
  {
  // Exist in. F(in,t=0) != F(in,t=1)
  main.addCNF(fcnf, 0);
  main.addCNF(fcnf, fcnf->numVars());
  
  Glucose::vec<Glucose::Lit> lit1, lit2; 
  lit1.push(main.genLit(fcnf->GetIndex(target), true));
  lit2.push(main.genLit(fcnf->GetIndex(target) + fcnf->numVars(), false));
  (main.solver)->addClause_( lit1 );
  (main.solver)->addClause_( lit2 );
  
  for (auto pin : circuit->inputs) {
    main.setEqual(fcnf->GetIndex(pin), fcnf->GetIndex(pin) + fcnf->numVars());
  }
  
  std::vector<int> outs1, outs2;
  for (auto pout : circuit->outputs) {
    outs1.push_back(fcnf->GetIndex(pout));
    outs2.push_back(fcnf->GetIndex(pout)+fcnf->numVars());
  }
  main.setNEqual(outs1, outs2);
  
  // Finish
  
  check.addCNF(fcnf, 0);
  check.addCNF(gcnf, fcnf->numVars());
  for (auto pin : circuit->inputs)
    check.setEqual(fcnf->GetIndex(pin), gcnf->GetIndex(pin) + fcnf->numVars());
  for (auto pout : circuit->outputs)
    check.setEqual(fcnf->GetIndex(pout), gcnf->GetIndex(pout) + fcnf->numVars());
}

void getDC::removeCase(std::map< nodecircuit::Node*, bool > *data, FType type) {
  if (type == NOTHING) {
    auto tmp = data->find(target);
    //assert((tmp == data->end());
    if (tmp->second == true)
      type = IN2;
    else
      type = IN1;
  }
  std::map < nodecircuit::Node*, bool >  folder;
  int cur_case = 0; // ADDED by Amir
  for(auto sig:*signals){
    bool is_one = folder[sig] = (*data)[sig];
    cur_case <<= 1;
    if (is_one)
      cur_case |= 0x00000001;
  }
  if (removed_cases.find(cur_case) != removed_cases.end()) {
    return; // Do not add if already added!
  }
  removed_cases.insert(cur_case);
  if (type == IN1)
    in1cases.push_back(folder);
  else
    in2cases.push_back(folder);

  Glucose::vec<Glucose::Lit> lits;
  for (auto sig : *signals) {
    //std::cout << fcnf->GetIndex(sig->name) << ": " << (*data)[sig] << std::endl;
    lits.push(main.genLit(fcnf->GetIndex(sig), !(*data)[sig]));
  }
  main.solver->addClause_(lits);

  return;
}

void getDC::removeCases_fromFile(std::string fname, FType type) {
  std::ifstream casefile; casefile.open(fname, std::ios::in);
  std::string str; std::string strtmp;
  getline(casefile, str); std::stringstream ss(str);

  std::vector<nodecircuit::Node *> index_name;
  std::map<nodecircuit::Node*, bool> signals;

  while (getline(ss, strtmp, ',')) {
    index_name.push_back(circuit->GetNode(strtmp));
  }

  while (getline(casefile, str)) {
    std::stringstream ss2(str);
    int i = 0;
    while (getline(ss2, strtmp, ',')) {
      signals[index_name[i]] = (strtmp.find("1") == -1) ? false : true;
      i++;
      if (index_name.size() == i) break;
    }
    removeCase(&signals, type);
    signals.clear();
  }
}

void getDC::solve() {
  while (1) {
    if (main.getAnswer(false) == false) {
      //std::cout << "no more in1/in2 cases" << std::endl;
      break; // no more cases, all the other input is Don't Care
    }else{
      int control = check.solver->nVars();
      for (auto pin : circuit->inputs) {
        Glucose::vec<Glucose::Lit> tmp;
        tmp.push(check.genLit(fcnf->GetIndex(pin), (main.modelNum(fcnf->GetIndex(pin)) == l__True) ? true: false ));
        tmp.push(check.genLit(control,false));
        check.solver->addClause_(tmp);
      }

      if (check.solver->solve( check.genLit(control, true) ) == false) {
        break;
        //assert((0);
      }else {
        std::map < nodecircuit::Node*, bool > tmp_case;
        for (auto sig : *signals) {
          tmp_case[sig] = (check.modelNum(fcnf->GetIndex(sig)) == l__True) ? true: false;
          //std::cout << sig->name << ": " << ((check.modelNum(fcnf->GetIndex(sig->name)) == l__True) ? true : false) << std::endl;
        }
        //std::cout << "Find case for " <<  ((check.modelNum(fcnf->GetIndex(target->name)) == l__True) ? "IN2" : "IN1" ) <<std::endl;
        removeCase(&tmp_case, check.modelNum(fcnf->GetIndex(target))==l__True? IN2: IN1 );
      }
      Glucose::vec<Glucose::Lit> tmp; tmp.push(check.genLit(control, false)); check.solver->addClause_(tmp);
    }
  }
}

int getDC::solve_additional()
{
  in2dccases = in2cases;
  in1dccases = in1cases;

  std::vector < std::map < nodecircuit::Node*, bool > > needCheck;
  for (int i = 0; i < (1 << signals->size()); i++) {
    std::map < nodecircuit::Node*, bool > tmp;
    for (int j = 0; j < signals->size(); j++) {
      tmp[(*signals)[j]] = i&(1 << j);
    }
    if (std::find(in1cases.begin(), in1cases.end(), tmp) == in1cases.end() && std::find(in2cases.begin(), in2cases.end(), tmp) == in2cases.end()) {
      needCheck.push_back(tmp);
    }
  }

  Glucose::GInterface solver;
  solver.addCNF(fcnf, 0);
  solver.addCNF(gcnf, fcnf->numVars());
  for (auto pin : circuit->inputs) {
    solver.setEqual(fcnf->GetIndex(pin), gcnf->GetIndex(pin) + fcnf->numVars());
  }
  std::vector<int> outs1, outs2;
  for (auto pout : circuit->outputs) {
    outs1.push_back(fcnf->GetIndex(pout));
    outs2.push_back(gcnf->GetIndex(pout) + fcnf->numVars());
  }
  solver.setNEqual(outs1, outs2);
  
  for (auto pat : needCheck) {
    Glucose::vec<Glucose::Lit> tmp;
    for (auto lit : pat)
      tmp.push(solver.genLit(fcnf->GetIndex(lit.first),lit.second));
    if (solver.getAnswer(tmp) == false) {
      dccases.push_back(pat);
      in1dccases.push_back(pat);
      in2dccases.push_back(pat);
    }
  }

  return dccases.size();
}

int getDC::solve_new() {
  //std::cout << "Don't call this function w/o Miyasaka's instruction!" << std::endl;

  std::vector< std::map < nodecircuit::Node*, bool > > ffcases;

  Glucose::GInterface solver;
  solver.addCNF(fcnf, 0);
  solver.addCNF(fcnf, fcnf->numVars());
  solver.addCNF(gcnf, fcnf->numVars() * 2);
  solver.solver->addClause(solver.genLit(fcnf->GetIndex(target), false));
  solver.solver->addClause(solver.genLit(fcnf->GetIndex(target) + fcnf->numVars(), true));
  for (auto pin : circuit->inputs) {
    solver.setEqual(fcnf->GetIndex(pin), fcnf->GetIndex(pin) + fcnf->numVars());
    solver.setEqual(fcnf->GetIndex(pin), gcnf->GetIndex(pin) + fcnf->numVars() * 2);
  }
  std::vector<int> outs1, outs2, specouts;
  for (auto pout : circuit->outputs) {
    outs1.push_back(fcnf->GetIndex(pout));
    outs2.push_back(fcnf->GetIndex(pout) + fcnf->numVars());
    specouts.push_back(gcnf->GetIndex(pout) + fcnf->numVars() * 2);
  }
  int control1 = solver.solver->nVars();
  int control2 = solver.solver->nVars()+ 1;
  int enable1 = solver.solver->nVars() + 2;
  int enable2 = solver.solver->nVars() + 3;
  int enable3 = solver.solver->nVars() + 4;
  int enable4 = solver.solver->nVars() + 5; solver.genLit(enable4, true);
  solver.ifTrue_thenEQ_elseNEQ(outs1, specouts, control1);
  solver.ifTrue_thenEQ_elseNEQ(outs2, specouts, control2);
  Glucose::SimpSolver *backup = new Glucose::SimpSolver(*solver.solver);

  // FF
  Glucose::vec < Glucose::Lit> assump_FF;
  assump_FF.push(solver.genLit(control1, false));
  assump_FF.push(solver.genLit(control2, false));
  assump_FF.push(solver.genLit(enable1, true));
  while (true) {
    bool res;
    struct itimerval timer, oldtimer;
    timer.it_value.tv_sec = MAXTIME; timer.it_value.tv_usec = 0; timer.it_interval.tv_sec = MAXTIME; timer.it_interval.tv_usec = 0;
    struct sigaction sa_sigint; memset(&sa_sigint, 0, sizeof(sa_sigint)); sa_sigint.sa_handler = Timeout; sa_sigint.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa_sigint, NULL); sigset_t block; sigemptyset(&block); sigaddset(&block, SIGALRM); sigprocmask(SIG_UNBLOCK, &block, NULL);
    int f = sigsetjmp(env_sigalrm_dc, 0);
    if (f == 0) {
      setitimer(ITIMER_REAL, &timer, &oldtimer);
      res = solver.getAnswer(assump_FF);
    }else {
      solver.solver->interrupt();
      delete solver.solver;
      main.solver = new Glucose::SimpSolver(*backup);
      res = false;
      std::cout << "Solver Timeout (Solve)" << std::endl;
    }
    setitimer(ITIMER_REAL, &oldtimer, NULL);
    signal(SIGALRM, SIG_DFL);
    if (res == false)
      break;

    std::map < nodecircuit::Node*, bool > tmp;
    Glucose::vec<Glucose::Lit> elim; 
    elim.push(solver.genLit(enable1, false));
    for (auto sig : *signals) {
      tmp[sig] = solver.modelNum(fcnf->GetIndex(sig)) == l__True ? true : false;
      elim.push(solver.genLit(fcnf->GetIndex(sig), (solver.modelNum(fcnf->GetIndex(sig)) == l__True ? false : true)));
      //std::cout << sig->name << ":" << (solver.modelNum(fcnf->GetIndex(sig)) == l__True);
    }
    //std::cout << std::endl;
    solver.solver->addClause(elim);
    ffcases.push_back(tmp);
  }

  // TF
  solver.solver->addClause(solver.genLit(enable1,false));
  Glucose::vec < Glucose::Lit> assump_TF;
  assump_TF.push(solver.genLit(control1, true));
  assump_TF.push(solver.genLit(control2, false));
  assump_TF.push(solver.genLit(enable2, true));
  while (true) {
    bool res;
    struct itimerval timer, oldtimer;
    timer.it_value.tv_sec = MAXTIME; timer.it_value.tv_usec = 0; timer.it_interval.tv_sec = MAXTIME; timer.it_interval.tv_usec = 0;
    struct sigaction sa_sigint; memset(&sa_sigint, 0, sizeof(sa_sigint)); sa_sigint.sa_handler = Timeout; sa_sigint.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa_sigint, NULL); sigset_t block; sigemptyset(&block); sigaddset(&block, SIGALRM); sigprocmask(SIG_UNBLOCK, &block, NULL);
    int f = sigsetjmp(env_sigalrm_dc, 0);
    if (f == 0) {
      setitimer(ITIMER_REAL, &timer, &oldtimer);
      res = solver.getAnswer(assump_TF);
    }
    else {
      solver.solver->interrupt();
      delete solver.solver;
      main.solver = new Glucose::SimpSolver(*backup);
      res = false;
      std::cout << "Solver Timeout (Solve)" << std::endl;
    }
    setitimer(ITIMER_REAL, &oldtimer, NULL);
    signal(SIGALRM, SIG_DFL);
    if (res == false)
      break;
    std::map < nodecircuit::Node*, bool > tmp;
    Glucose::vec<Glucose::Lit> elim;
    elim.push(solver.genLit(enable2, false));
    for (auto sig : *signals) {
      tmp[sig] = solver.modelNum(fcnf->GetIndex(sig)) == l__True ? true : false;
      elim.push(solver.genLit(fcnf->GetIndex(sig), (solver.modelNum(fcnf->GetIndex(sig)) == l__True ? false : true)));
      //std::cout << sig->name << ":" << (solver.modelNum(fcnf->GetIndex(sig)) == l__True);
    }
    //std::cout << std::endl;
    solver.solver->addClause(elim);
    in1cases.push_back(tmp);
  }

  // FT
  solver.solver->addClause(solver.genLit(enable2, false));
  Glucose::vec < Glucose::Lit> assump_FT;
  assump_FT.push(solver.genLit(control1, false));
  assump_FT.push(solver.genLit(control2, true));
  assump_FT.push(solver.genLit(enable3, true));
  while (true) {
    bool res;
    struct itimerval timer, oldtimer;
    timer.it_value.tv_sec = MAXTIME; timer.it_value.tv_usec = 0; timer.it_interval.tv_sec = MAXTIME; timer.it_interval.tv_usec = 0;
    struct sigaction sa_sigint; memset(&sa_sigint, 0, sizeof(sa_sigint)); sa_sigint.sa_handler = Timeout; sa_sigint.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa_sigint, NULL); sigset_t block; sigemptyset(&block); sigaddset(&block, SIGALRM); sigprocmask(SIG_UNBLOCK, &block, NULL);
    int f = sigsetjmp(env_sigalrm_dc, 0);
    if (f == 0) {
      setitimer(ITIMER_REAL, &timer, &oldtimer);
      res = solver.getAnswer(assump_FT);
    }
    else {
      solver.solver->interrupt();
      delete solver.solver;
      main.solver = new Glucose::SimpSolver(*backup);
      res = false;
      std::cout << "Solver Timeout (Solve)" << std::endl;
    }
    setitimer(ITIMER_REAL, &oldtimer, NULL);
    signal(SIGALRM, SIG_DFL);
    if (res == false)
      break;
    std::map < nodecircuit::Node*, bool > tmp;
    Glucose::vec<Glucose::Lit> elim; elim.push(solver.genLit(enable3, false));
    for (auto sig : *signals) {
      tmp[sig] = solver.modelNum(fcnf->GetIndex(sig)) == l__True ? true : false;
      elim.push(solver.genLit(fcnf->GetIndex(sig), (solver.modelNum(fcnf->GetIndex(sig)) == l__True ? false : true)));
     // std::cout << sig->name << ":" << (solver.modelNum(fcnf->GetIndex(sig)) == l__True);
    }
    //std::cout << std::endl;
    solver.solver->addClause(elim);
    in2cases.push_back(tmp);
  }

  // TT
  solver.solver->addClause(solver.genLit(enable3, false));
  Glucose::vec < Glucose::Lit> assump_TT;
  assump_TT.push(solver.genLit(control1, true));
  assump_TT.push(solver.genLit(control2, true));
  assump_TT.push(solver.genLit(enable4, true));
  while (true) {
    bool res;
    struct itimerval timer, oldtimer;
    timer.it_value.tv_sec = MAXTIME; timer.it_value.tv_usec = 0; timer.it_interval.tv_sec = MAXTIME; timer.it_interval.tv_usec = 0;
    struct sigaction sa_sigint; memset(&sa_sigint, 0, sizeof(sa_sigint)); sa_sigint.sa_handler = Timeout; sa_sigint.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa_sigint, NULL); sigset_t block; sigemptyset(&block); sigaddset(&block, SIGALRM); sigprocmask(SIG_UNBLOCK, &block, NULL);
    int f = sigsetjmp(env_sigalrm_dc, 0);
    if (f == 0) {
      setitimer(ITIMER_REAL, &timer, &oldtimer);
      res = solver.getAnswer(assump_TT);
    }
    else {
      solver.solver->interrupt();
      delete solver.solver;
      main.solver = new Glucose::SimpSolver(*backup);
      res = false;
      std::cout << "Solver Timeout (Solve)" << std::endl;
    }
    setitimer(ITIMER_REAL, &oldtimer, NULL);
    signal(SIGALRM, SIG_DFL);
    if (res == false)
      break;
    std::map < nodecircuit::Node*, bool > tmp;
    Glucose::vec<Glucose::Lit> elim; elim.push(solver.genLit(enable4, false));
    for (auto sig : *signals) {
      tmp[sig] = solver.modelNum(fcnf->GetIndex(sig)) == l__True ? true : false;
      elim.push(solver.genLit(fcnf->GetIndex(sig), (solver.modelNum(fcnf->GetIndex(sig)) == l__True ? false : true)));
      //std::cout << sig->name << ":" << (solver.modelNum(fcnf->GetIndex(sig)) == l__True);
    }
    //std::cout << std::endl;
    solver.solver->addClause(elim);
    dccases.push_back(tmp);
  }

  //2
  for (auto tmp : ffcases) {
    for (int i = 0; i < in1cases.size(); i++) {
      if (in1cases[i] == tmp) {
        in1cases[i] = in1cases[in1cases.size() - 1];
        in1cases.pop_back();
      }
    }
    for (int i = 0; i < in2cases.size(); i++) {
      if (in2cases[i] == tmp) {
        in2cases[i] = in2cases[in2cases.size() - 1];
        in2cases.pop_back();
      }
    }
    for (int i = 0; i < dccases.size(); i++) {
      if (dccases[i] == tmp) {
        dccases[i] = dccases[dccases.size() - 1];
        dccases.pop_back();
      }
    }
  }

  //3
  std::vector<std::map < nodecircuit::Node*, bool >> list_inter;
  for (auto line1 : in1cases) {
    for (auto line2 : in2cases) {
      if (line1 == line2) {
        list_inter.push_back(line1);
      }
    }
  }
  for (auto tmp : list_inter) {
    //for (auto cel : tmp)
    //  std::cout << "inter:" << cel.first->name << ":" << cel.second << ",";
    std::cout << std::endl;

    for (int i = 0; i < in1cases.size(); i++) {
      if (in1cases[i] == tmp) {
        in1cases[i] = in1cases[in1cases.size() - 1];
        in1cases.pop_back();
      }
    }
    for (int i = 0; i < in2cases.size(); i++) {
      if (in2cases[i] == tmp) {
        in2cases[i] = in2cases[in2cases.size() - 1];
        in2cases.pop_back();
      }
    }
    for (int i = 0; i < dccases.size(); i++) {
      if (dccases[i] == tmp) {
        dccases[i] = dccases[dccases.size() - 1];
        dccases.pop_back();
      }
    }
  }

  //4
  for (auto tmp : in1cases) {
    for (int i = 0; i < dccases.size(); i++) {
      if (dccases[i] == tmp) {
        dccases[i] = dccases[dccases.size() - 1];
        dccases.pop_back();
      }
    }
  }

  //5
  for (auto tmp : in2cases) {
    for (int i = 0; i < dccases.size(); i++) {
      if (dccases[i] == tmp) {
        dccases[i] = dccases[dccases.size() - 1];
        dccases.pop_back();
      }
    }
  }

  //6
  in1dccases = in1cases;
  std::copy(dccases.begin(), dccases.end(), std::back_inserter(in1dccases));

  in2dccases = in2cases;
  std::copy(dccases.begin(), dccases.end(), std::back_inserter(in2dccases));

}

void getDC::dumpCases() {
  std::cout << "### IN1 CASES ###" << std::endl;
  if(in1cases.size()){
    for(auto itr = in1cases[0].begin(); itr!=in1cases[0].end(); itr++){
      std::cout << itr->first->name << ",";
    }
    std::cout << std::endl;
  }
  
  for (auto itr = in1cases.begin(); itr != in1cases.end(); itr++) {
    for (auto item = (*itr).begin(); item != (*itr).end(); item++) {
      std::cout << item->second << ",";
    }
    std::cout << std::endl;
  }

  std::cout << "### IN2 CASES ###" << std::endl;
  if(in2cases.size()){
    for(auto itr = in2cases[0].begin(); itr!=in2cases[0].end(); itr++){
      std::cout << itr->first->name << ",";
    }
    std::cout << std::endl;
  }
  
  for (auto itr = in2cases.begin(); itr != in2cases.end(); itr++) {
    for (auto item = (*itr).begin(); item != (*itr).end(); item++) {
      std::cout << item->second << ",";
    }
    std::cout << std::endl;
  }

  std::cout << "### DC CASES ###" << std::endl;
  if (dccases.size()) {
    for (auto itr = dccases[0].begin(); itr != dccases[0].end(); itr++) {
      std::cout << itr->first->name << ",";
    }
    std::cout << std::endl;
  }

  for (auto itr = dccases.begin(); itr != dccases.end(); itr++) {
    for (auto item = (*itr).begin(); item != (*itr).end(); item++) {
      std::cout << item->second << ",";
    }
    std::cout << std::endl;
  }

  std::cout << "### IN2DC CASES ###" << std::endl;
  if (in2dccases.size()) {
    for (auto itr = in2dccases[0].begin(); itr != in2dccases[0].end(); itr++) {
      std::cout << itr->first->name << ",";
    }
    std::cout << std::endl;
  }

  for (auto itr = in2dccases.begin(); itr != in2dccases.end(); itr++) {
    for (auto item = (*itr).begin(); item != (*itr).end(); item++) {
      std::cout << item->second << ",";
    }
    std::cout << std::endl;
  }
}

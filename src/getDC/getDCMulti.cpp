#include <cmath>
#include <algorithm>
#include "getDCMulti.h"

using namespace DC;

getDCMulti::getDCMulti(nodecircuit::NodeVector * signals, nodecircuit::NodeVector * targets, nodecircuit::Circuit * impl, nodecircuit::Circuit * spec) :
  implcnf("impl", impl, targets, signals), speccnf("spec", spec, targets, signals),
  signals(signals), targets(targets), impl(impl), spec(spec)
{
  main.addCNF(&implcnf, 0);
  main.addCNF(&implcnf, implcnf.numVars());
  main.addCNF(&speccnf, implcnf.numVars()*2);
  
  //input is the same
  for (auto nd : impl->inputs) {
    main.setEqual(implcnf.GetIndex(nd), implcnf.GetIndex(nd) + implcnf.numVars());
    main.setEqual(implcnf.GetIndex(nd), speccnf.GetIndex(nd) + implcnf.numVars()*2);
  }
  for (auto nd : *targets)
    main.solver->addClause(main.genLit(implcnf.GetIndex(nd) + implcnf.numVars(), false));

  std::vector<int> outs1, outs2, outs3;
  for (auto pout : impl->outputs) {
    outs1.push_back(implcnf.GetIndex(pout));
    outs2.push_back(implcnf.GetIndex(pout) + implcnf.numVars());
    outs3.push_back(speccnf.GetIndex(pout) + implcnf.numVars()*2);
  }
  main.setEqual(outs1, outs3);
  main.setNEqual(outs2, outs3);

  std::vector< std::map < nodecircuit::Node*, bool > > tmp;
  for (int i = 0; i < targets->size(); i++)
    in2cases.push_back(tmp);

  Solve();
}

void getDCMulti::Elim(std::map<nodecircuit::Node*, bool> ca) {

  Glucose::vec<Glucose::Lit> tmp;
  for (auto sig : *signals) {
    tmp.push(main.genLit(implcnf.GetIndex(sig), !ca[sig]));
  }
  main.solver->addClause(tmp);
  return;
}

void getDCMulti::Solve()
{
  while (main.solver->solve()) {
    uint64_t index = 0;
    std::map<nodecircuit::Node*, bool> tmp;
    for (auto sig : *signals) {
      tmp[sig] = (main.modelNum(implcnf.GetIndex(sig)) == l__True ? true : false);
    }

    for (int i = 0; i < targets->size();i++) {
      in2cases[i].push_back(tmp);
    }
    
    Elim(tmp);
  }
  return;
}

void DC::Dump(getDCMulti &t, nodecircuit::NodeVector* targets)
{
  std::cout << "### getDCMulti DUMP" << std::endl;
  for (uint64_t i = 0; i < targets->size(); i++) {
    std::cout << "##" << (*targets)[i]->name << std::endl;
    if (t.in2cases[i].size() == 0) {
      std::cout << "No pattern" << std::endl;
      continue;
    }
    for (auto cel : t.in2cases[i][0])
      std::cout << cel.first->name << " ";
    std::cout << std::endl;

    for (auto line : t.in2cases[i]) {
      for (auto cel : line)
        std::cout << cel.second;
      std::cout << " 1" << std::endl;
    }
  }
}

uint64_t DC::pow2(uint64_t a)
{
  assert(a >= 0 && a <= 30);
  return 1 << a;
}

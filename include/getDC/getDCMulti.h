#ifndef GETDC_MULTI_H_
#define GETDC_MULTI_H_

#include <vector>
#include <map>

#include "node.h"
#include "glucoseInterface.h"
#include "CNFGenerator.h"

namespace DC {

  class getDCMulti {
  public:

    getDCMulti(nodecircuit::NodeVector* signals, nodecircuit::NodeVector* targets, nodecircuit::Circuit* impl, nodecircuit::Circuit* spec);

    std::vector<std::vector< std::map < nodecircuit::Node*, bool > >> in2cases;

  private:
    CNFgen::CNFGenerator implcnf;
    CNFgen::CNFGenerator speccnf;
    Glucose::GInterface main;

    nodecircuit::NodeVector* signals;
    nodecircuit::NodeVector* targets;
    nodecircuit::Circuit* impl;
    nodecircuit::Circuit* spec;

    void Elim(std::map<nodecircuit::Node*, bool> tmp);
    void Solve();
  };

  void Dump(getDCMulti &t, nodecircuit::NodeVector* targets);
  inline uint64_t pow2(uint64_t a);
}

#endif
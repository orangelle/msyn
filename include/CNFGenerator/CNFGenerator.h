//
// Created by iwata on 17/06/22.
// Modified by Kimura for depag project
//

#ifndef DEPAG_CNFGENERATOR_H
#define DEPAG_CNFGENERATOR_H

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iterator>
#include "simp/SimpSolver.h"
#include "core/SolverTypes.h"
#include "mtl/Vec.h"
#include "node.h"
#include "utils/System.h"
#include "core/Dimacs.h"
#include "simp/SimpSolver.h"

#include "base/main/mainInt.h"
#include "base/main/main.h"
#include "base/abc/abc.h"


namespace CNFgen {

  enum NodeType {
    BUFF,
    NOT,
    AND,
    NAND,
    OR,
    NOR,
    XOR,
    XNOR,
    VDD,
    GND
  };

  class CNFGenerator {

  public:
	  std::vector<std::map<int,bool>> clauses;
	  nodecircuit::Circuit* circuit_org;

    CNFGenerator();
    CNFGenerator(std::string name, nodecircuit::Circuit *c, nodecircuit::NodeVector *ex_in, nodecircuit::NodeVector *ex_out);
    ~CNFGenerator(){};
    int Init(std::string name, nodecircuit::Circuit *c, nodecircuit::NodeVector *ex_in, nodecircuit::NodeVector *ex_out);

    int Circuit2Abc(ABC::Abc_Ntk_t *pNtk, nodecircuit::Circuit *c, nodecircuit::NodeVector *ex_in, nodecircuit::NodeVector *ex_out);
    int Abc2CNF(ABC::Abc_Ntk_t *pNtk);
    int Abc2CNF2(ABC::Abc_Ntk_t *pNtk);
	void dumpcnf();
    void Finalize(nodecircuit::NodeVector *ex_in, nodecircuit::NodeVector *ex_out);
    int numVars() {
      return signals.size();
    };

    int GetIndex(std::string signal);
    int GetIndex(nodecircuit::Node * nd);

    void writeDimacs(std::string filename);
    std::unordered_map<std::string, int> signals;

    void Dump();


  private:
    std::map<std::string, int> cache_string;
    std::map<nodecircuit::Node *, int> cache_Node;
  
  }; //CNFGenerator

}
#endif //DEPAG_CNFGENERATOR_H

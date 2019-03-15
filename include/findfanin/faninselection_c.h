#ifndef FANIN_SELECTION_C
#define FANIN_SELECTION_C
#include "node.h"
#include "glucoseInterface.h"
#include "CNFGenerator.h"
#include "setcover.h"

using namespace nodecircuit;

class FaninSelectorC
{
public:
	FaninSelectorC(Circuit* spec, Circuit* impl, NodeVector* targets, NodeVector* cands, NodeVector* solution_nodes, int iteration_limit);
	~FaninSelectorC() {};
	bool MakeSolver();
	bool solve();

private:
	CNFgen::CNFGenerator impl;
	CNFgen::CNFGenerator spec;
	NodeVector* cands;
	NodeVector* targets;
	NodeVector* inputs;
	NodeVector* outputs;
	Glucose::GInterface main;
	int num_t;
	int num_cnf;
};

#endif

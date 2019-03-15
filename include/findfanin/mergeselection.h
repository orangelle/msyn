#ifndef LOGICOPT_FINDFANIN_MERGESELECTION
#define LOGICOPT_FINDFANIN_MERGESELECTION

#include "node.h"
#include "glucoseInterface.h"
#include "setcover_m.h"

using namespace nodecircuit;

class MergeSelection
{
public:
	enum FType
	{
		NOTHING,
		IN0,
		IN1
	};
	MergeSelection(Circuit* impl_circuit, Circuit* spec_circuit,Node* seed, NodeVector* candidates, NodeVector* candidate_inputs);
	~MergeSelection() {};
	NodeVector solution;
	NodeSet solution_inputs;
	std::vector<std::map<nodecircuit::Node *, bool>> onset;

private:
	Node* seed;
	Circuit* impl_circuit;
	NodeVector cands;
	NodeVector cand_inputs;
	CNFgen::CNFGenerator impl;
	CNFgen::CNFGenerator spec;
	Glucose::GInterface glu;
	std::vector<std::map<nodecircuit::Node *, bool>> in0cases;
	std::vector<std::map<nodecircuit::Node *, bool>> in1cases;
	map<Node *, int> eq_trigger;
	int en_in0, en_in1;
	setcover_m* set;
	void MakeSolver();
	void Solve();
	void removeCase(std::map<nodecircuit::Node *, bool> data, FType = NOTHING);
	void GetOnSet(Circuit* impl_circuit);
};

#endif

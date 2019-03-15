#ifndef LOGICOPT_CIRCUTI_NODEMERGE
#define LOGICOPT_CIRCUIT_NODEMERGE

#include "node.h"
#include "glucoseInterface.h"
#include "setcover_m.h"

using namespace nodecircuit;

class  NodeMerge
{
public:
	NodeMerge(Circuit* impl_circuit, Circuit* spec_circuit);
	~NodeMerge() {};
	void Run();
	int total_num;
	int one_num;
	int two_num;
	int three_num;

private:
	nodecircuit::Circuit* impl_circuit;
	nodecircuit::Circuit* spec_circuit;
	nodecircuit::NodeVector nodes_nc;
	nodecircuit::NodeVector nodes_tra;
	void InitCand(Node* seed, NodeVector& cands, NodeVector& cand_inputs);// must be used before setting the target
};

#endif

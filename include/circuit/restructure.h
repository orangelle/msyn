#ifndef LOGICOPT_CIRCUIT_NODE_RESTRUCTOR_H
#define LOGICOPT_CIRCUIT_NODE_RESTRUCTOR_H

#include "node.h"


class NodeRestructurer
{
public:
	NodeRestructurer(nodecircuit::Circuit* impl_circuit, nodecircuit::Circuit* spec_circuit);
	~NodeRestructurer() {}
	void Traverse(int num);		//treat every combination of targets
	void Traverse(int num, nodecircuit::NodeVector initial_targets);
	void RunOnce(nodecircuit::NodeVector selected_targets);
	std::vector<nodecircuit::NodeVector> combs;

private:
	nodecircuit::Circuit* impl_circuit;
	nodecircuit::Circuit* spec_circuit;
	nodecircuit::NodeVector all_nodes;
	nodecircuit::NodeVector comb;
	int num;
	int times;
	//traverse all the the combination
	//void Traverse(nodecircuit::NodeVector seleted_nodes, nodecircuit::NodeSet PO_set, nodecircuit::NodeSet fanout_set, int pos, int num);
	void Traverse(nodecircuit::NodeVector selected_nodes, nodecircuit::NodeSet po_set, nodecircuit::NodeSet fanout_set, int pos, int num, nodecircuit::NodeVector initial_targets);
	//group the targets according to their PO sharing
	void Group(std::vector<nodecircuit::NodeSet*> &dummy_targets_nodes, std::vector<nodecircuit::NodeSet*> &dummy_targets_fanin_cone_old,
		std::vector<nodecircuit::NodeSet*> &dummy_targets_fanin_cone_new, std::vector<nodecircuit::NodeSet*> &dummy_targets_fanin_pi,
		std::vector<nodecircuit::NodeSet*> &dummy_targets_fanout_cone, std::vector<nodecircuit::NodeSet*> &dummy_targets_fanout_po);
	//main process
	int Process(std::vector<nodecircuit::NodeSet*> &dummy_targets_nodes, std::vector<nodecircuit::NodeSet*> &dummy_targets_fanin_cone,
		std::vector<nodecircuit::NodeSet*> &dummy_targets_fanout_cone,
		std::vector<nodecircuit::NodeSet*> &dummy_targets_fanout_po);
	void CreateInitialCandidates(nodecircuit::NodeSet *target_fanin_nodes, nodecircuit::NodeVector &candidates, unsigned long &num_candids);
	//to judge whether target_node can be the next selected target, and update the po_set and fanout_set
	bool Judge(nodecircuit::NodeSet po_set, nodecircuit::NodeSet fanout_set, nodecircuit::NodeVector selected_nodes, nodecircuit::Node* target_node);
	void GetCombs(int num, int pos);
	bool InOneGroup(nodecircuit::NodeVector targets);
	bool FindSolution(nodecircuit::Circuit *spec_circuit, nodecircuit::Circuit* impl_circuit, nodecircuit::NodeVector* candidates, nodecircuit::NodeVector* targets,
		nodecircuit::NodeVector &solution_nodes, std::vector< std::map < nodecircuit::Node*, bool > > &in2cases, int iteration_limit);
};



#endif
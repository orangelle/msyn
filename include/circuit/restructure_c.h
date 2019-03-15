#ifndef LOGICOPT_CIRCUIT_NODE_RESTRUCTOR_C_H
#define LOGICOPT_CIRCUIT_NODE_RESTRUCTOR_C_H

#include "node.h"


class ClusterRestructurer
{
private:
	nodecircuit::Circuit* impl_circuit;
	nodecircuit::Circuit* spec_circuit;
	int times;
	int iteration_limit;
	bool solution_found;
public:
	ClusterRestructurer(nodecircuit::Circuit* impl_circuit, nodecircuit::Circuit* spec_circuit) {}
	~ClusterRestructurer() {}
	void SortByOrder();
	void Run();
	void RunOnce(nodecircuit::Cluster* cluster);
};

#endif
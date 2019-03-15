#ifndef FANIN_SELECTION
#define FANIN_SELECTION
#include "node.h"
#include "glucoseInterface.h"
#include "setcover.h"
namespace fanin_selection
{
class FaninSelector
{
public:
	enum FType
	{
		NOTHING,
		IN0,
		IN1
	};
	FaninSelector(nodecircuit::Circuit *impl_circuit, nodecircuit::Circuit *spec_circuit, nodecircuit::NodeVector *candidates, nodecircuit::NodeVector *targets, int iteration_limit);
	~FaninSelector(){};
	nodecircuit::NodeVector solution;
	std::vector<std::map<nodecircuit::Node *, bool>> in0cases;
	std::vector<std::map<nodecircuit::Node *, bool>> in1cases;

protected:
	SC::setcover *set;
	CNFgen::CNFGenerator impl;
	CNFgen::CNFGenerator spec;
	nodecircuit::Circuit *impl_circuit;
	nodecircuit::NodeVector *candidates;
	nodecircuit::NodeVector *targets;
	Glucose::GInterface main;
	int iteration_limit;
	bool makeSolver();
	bool solve();
	void removeCase(std::map<nodecircuit::Node *, bool> data, FType = NOTHING);
	int en_in0, en_in1;
	std::map<nodecircuit::Node *, int> eq_trigger;
};

} // namespace fanin_selection

#endif
#ifndef DUAL_FIND_FANIN
#define DUAL_FIND_FANIN

#include "node.h"
#include "glucoseInterface.h"
#include "setcover.h"

namespace dual{

	class DualFindFanin{
	public:
		enum FType {
			NOTHING = 0,
			IN1 = 1,
			IN2 = 2,
		};
		DualFindFanin(nodecircuit::Circuit *impl_circuit, nodecircuit::Circuit *spec_circuit, nodecircuit::NodeVector *candidates,
			nodecircuit::NodeVector *targets, bool initial_check, bool in1in2check, int iteration_limit, unsigned int TIMELIMIT);
		~DualFindFanin() {};
		nodecircuit::NodeVector solution;
		std::vector < std::map < nodecircuit::Node*, bool > > in1cases_t1;
		std::vector < std::map < nodecircuit::Node*, bool > > in2cases_t1;
		std::vector < std::map < nodecircuit::Node*, bool > > in1cases_t2;
		std::vector < std::map < nodecircuit::Node*, bool > > in2cases_t2;
		SC::setcover *set;
		void solve(int timelimit);
		
	protected:
		void removeCase(Glucose::GInterface &solver, std::map < nodecircuit::Node*, bool > data, FType type = NOTHING);
		void makeSolver(Glucose::GInterface &slover, int target1, int target2);
		void makeChecker() {};
		CNFgen::CNFGenerator impl;
		CNFgen::CNFGenerator spec;
		nodecircuit::Circuit *impl_circuit;
		nodecircuit::NodeVector *candidates;
		nodecircuit::NodeVector *targets;
		Glucose::GInterface solver_t1;
		Glucose::GInterface solver_t2;
		Glucose::GInterface checker;
		std::map<nodecircuit::Node*, int> eq_trigger_t1;
		std::map<nodecircuit::Node*, int> eq_trigger_t2;
		int maxtime;
		bool t1 = false, t2 = false, t1_t2 = true, sol_t1_found = false, sol_t2_found = false;
	};
}
#endif

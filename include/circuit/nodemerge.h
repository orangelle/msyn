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
	void SelectMerge(int i);
	void QuickMerge();
	void FaninMerge();
	//void RunOnce(string name);
	int total_cnt;
	int one_cnt;
	int two_cnt;
	int three_cnt;
	int quick_cnt;
	int fanin_cnt;

private:
	Circuit* impl_circuit;
	Circuit* spec_circuit;
	NodeVector nodes_nc;
	NodeVector nodes_tra;
	//ClusterVector clu_tra;
	int limit_in;
	void InitCand(Node* seed, NodeVector& cands, NodeVector& cand_inputs);// must be used before setting the target
	void InitCand(Cluster* seed, ClusterVector &cand_clus, NodeVector& cand_inpus);//cluster version
};

#endif

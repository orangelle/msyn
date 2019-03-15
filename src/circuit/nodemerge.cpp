#include <iostream>
#include <algorithm>
#include <csetjmp>
#include <csignal>
#include <ctime>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>
#include "nodemerge.h"
#include "mergeselection.h"

using namespace std;

NodeMerge::NodeMerge(Circuit* impl_circuit, Circuit* spec_circuit) {
	this->impl_circuit = impl_circuit;
	this->spec_circuit = spec_circuit;
	for (auto node : impl_circuit->all_nodes) {
		if (node->is_input || node->is_output)
			continue;
		nodes_nc.push_back(node);
		nodes_tra.push_back(node);
	}
	total_num = nodes_nc.size();
	cout << "number of the nodes to cluster: " << total_num << endl;
}

void NodeMerge::Run() {
	while (!nodes_tra.empty()) {
		Node* seed = nodes_tra.back();
		cout << "\n==============================\n seed: " << seed->name << endl;
		nodes_tra.pop_back();
		NodeVector candidates;
		NodeVector candidate_inputs;
		InitCand(seed, candidates, candidate_inputs);//before setting the target
		impl_circuit->SetTargets({ seed });
		MergeSelection MergeSelector(impl_circuit, spec_circuit, seed, &candidates, &candidate_inputs);
		
		NodeVector tmp;
		tmp.push_back(seed);
		impl_circuit->Restore(tmp);

		if (MergeSelector.solution.size() == 1) {
			one_num++;
		}
		else if (MergeSelector.solution.size() == 2) {
			two_num++;
		}
		else if (MergeSelector.solution.size() == 3) {
			three_num++;
		}

		/*//if there is no solution
		if (MergeSelector.solution.empty()) {
			NodeVector tmp;
			tmp.push_back(seed);
			impl_circuit->Restore(tmp);
			continue;
		}

		//apply the merging on the circuit
		impl_circuit->Restruct(seed, MergeSelector.onset);
		NodeVector nodes_merged;
		nodes_merged.push_back(seed);
		nodes_merged.insert(nodes_merged.end(), MergeSelector.solution.begin(), MergeSelector.solution.end());
		impl_circuit->MakeCluster(nodes_merged);*/

		//delete the merged nodes from nodes_nc
		/*for (Node* node : nodes_merged) {
			nodes_nc.erase(find(nodes_nc.begin(), nodes_nc.end(), node));
			nodes_tra.erase(find(nodes_tra.begin(), nodes_tra.end(), node));
		}
		*/

		//break; //test
	}
}

//initialize the candidates and impl's POs(filter the POs effected by seed)
void NodeMerge::InitCand(Node* seed, NodeVector& cands, NodeVector& cand_inputs) {
	cands.clear();
	cand_inputs.clear();

	NodeSet TFO_of_seed, PO_of_seed, cands_set, cand_inputs_set;

	impl_circuit->GetFanoutCone(seed, TFO_of_seed, PO_of_seed);

	//shrink the PO;
	impl_circuit->outputs.clear();
	impl_circuit->outputs.insert(impl_circuit->outputs.begin(), PO_of_seed.begin(), PO_of_seed.end());

	for (Node* PO_node : PO_of_seed) {
		NodeSet TFI_of_PO, PI_of_PO;
		impl_circuit->GetFaninCone(PO_node, TFI_of_PO, PI_of_PO);
		for (Node* TFI_node : TFI_of_PO) {
			if (TFO_of_seed.find(TFI_node) == TFO_of_seed.end() && !TFI_node->is_input
				&& find(nodes_nc.begin(), nodes_nc.end(), TFI_node) != nodes_nc.end()) {//if not in TFO of seed && not PI && in nodes_c
				cands_set.insert(TFI_node);
				cand_inputs_set.insert(TFI_node->inputs.begin(), TFI_node->inputs.end());
			}
		}
	}
	//convert to vector
	cands.insert(cands.begin(), cands_set.begin(), cands_set.end());
	cand_inputs.insert(cand_inputs.begin(), cand_inputs_set.begin(), cand_inputs_set.end());
	//show the info
	cout << "cands: " << cands.size() << endl;
	cout << "cand_inputs: " << cand_inputs.size() << endl;
	//cout << "candidates: ";
	//for (Node* node : cands) {
	//	cout << node->name << ", ";
	//}
	//cout << endl;
	//cout << "candidate inputs: ";
	//for (Node* node : cand_inputs) {
	//	cout << node->name << ", ";
	//}
	//cout << endl;
}



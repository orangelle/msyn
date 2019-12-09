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

//NodeMerge::NodeMerge(Circuit* impl_circuit, Circuit* spec_circuit) {
//	this->impl_circuit = impl_circuit;
//	this->spec_circuit = spec_circuit;
//	for (auto node : impl_circuit->all_nodes) {
//		if (node->is_input || node->is_output)
//			continue;
//		nodes_nc.push_back(node);
//		nodes_tra.push_back(node);
//	}
//	total_num = nodes_nc.size();
//	cout << "number of the nodes to cluster: " << total_num << endl;
//}

NodeMerge::NodeMerge(Circuit* impl_circuit, Circuit* spec_circuit) {
	this->impl_circuit = impl_circuit;
	this->spec_circuit = spec_circuit;
	limit_in = 6;
}

//void NodeMerge::RunOnce(string name) {
//	Node* seed = impl_circuit->all_nodes_map[name];
//	cout << "\n==============================\n seed: " << seed->name << endl;
//	NodeVector candidates;
//	NodeVector candidate_inputs;
//	InitCand(seed, candidates, candidate_inputs);//before setting the target
//	impl_circuit->SetTargets({ seed });
//	MergeSelection MergeSelector(impl_circuit, spec_circuit, seed, &candidates, &candidate_inputs);
//}

void NodeMerge::SelectMerge(int lmt) {
	//for (int i = impl_circuit->all_clusters.size() - 1; i >= 0; i--) {
		//clu_nc.push_back(clu);
		//clu_tra.push_back(impl_circuit->all_clusters.at(i));
	//}
	one_cnt = 0;
	two_cnt = 0;
	three_cnt = 0;
	total_cnt = impl_circuit -> all_clusters.size();
	cout << "number of the nodes to cluster: " << total_cnt << endl;
	ClusterUnset::iterator it = impl_circuit->all_clusters.begin();
	while (it!=impl_circuit->all_clusters.end()){
	//while (!clu_tra.empty()) {
		//get a seed cluster
		//Cluster* seed = clu_tra.back();
		Cluster* seed = *it;
		//clu_tra.pop_back();
		cout << "\n==============================\nseed" << seed->name << ": ";
		for (Node* node : seed->nodes_c)
			cout << node->name << " ";
		cout << endl;
		//do selection only for single node
		if (seed->nodes_c.size() == 1 && (seed->nodes_c.at(0)->HasOutput() || seed->nodes_c.at(0)->is_output)) {
			ClusterVector candidates;
			NodeVector candidate_inputs;
			InitCand(seed, candidates, candidate_inputs);//before setting the target
			impl_circuit->SetTargets(seed->nodes_c);
			int limit = lmt;
			MergeSelection MergeSelector(impl_circuit, spec_circuit, seed, &candidates, &candidate_inputs, limit);

			if (MergeSelector.solution_c.size() == 1) {
				Cluster* clu_sol = MergeSelector.solution_c.at(0);
				one_cnt++;
				cout << "Get the on-set for " << seed->name << endl;
				MergeSelector.GetOnSet();//calculate the on set
				cout << "restructure the node " << seed->name << endl;
				impl_circuit->Restruct(seed->nodes_c[0], MergeSelector.onset);
				//clu_nc.erase(find(clu_nc.begin(), clu_nc.end(), seed));
				it = impl_circuit->Merge(clu_sol, seed);
				//remove the nodes in fanin cone which is not used any more
				for (Node* nd : seed->nodes_c[0]->inputs_pre) 
					if (nd->outputs.empty() && !nd->is_input && !nd->is_output)
						impl_circuit->Remove(nd);
			}
			else if (limit >= 2 && MergeSelector.solution_c.size() == 2 && MergeSelector.solution_inputs.size() <= limit_in) {
				two_cnt++;
				cout << "Get the on-set for " << seed->name << endl;
				MergeSelector.GetOnSet();//calculate the on set
				cout << "restructure the node " << seed->name << endl;
				impl_circuit->Restruct(seed->nodes_c[0], MergeSelector.onset);
				ClusterVector::iterator clst_itr = MergeSelector.solution_c.begin();
				Cluster* tar = *clst_itr;
				clst_itr++;
				for (; clst_itr != MergeSelector.solution_c.end(); clst_itr++) {
					//if (find(clu_tra.begin(), clu_tra.end(), *clst_itr) != clu_tra.end())
					//	clu_tra.erase(find(clu_tra.begin(), clu_tra.end(), *clst_itr));
					impl_circuit->Merge(tar, *clst_itr);
				}
				it = impl_circuit->Merge(tar, seed);
				//remove the nodes in fanin cone which is not used any more
				for (Node* nd : seed->nodes_c[0]->inputs_pre)
					if (nd->outputs.empty() && !nd->is_input && !nd->is_output)
						impl_circuit->Remove(nd);
			}
			else if (limit >= 3 && MergeSelector.solution_c.size() == 3 && MergeSelector.solution_inputs.size() <= limit_in) {
				three_cnt++;
				cout << "Get the on-set for " << seed->name << endl;
				MergeSelector.GetOnSet();//calculate the on set
				cout << "restructure the node " << seed->name << endl;
				impl_circuit->Restruct(seed->nodes_c[0], MergeSelector.onset);
				ClusterVector::iterator clst_itr = MergeSelector.solution_c.begin();
				Cluster* tar = *clst_itr;
				clst_itr++;
				for (; clst_itr != MergeSelector.solution_c.end(); clst_itr++) {
					//if (find(clu_tra.begin(), clu_tra.end(), *clst_itr) != clu_tra.end())
					//	clu_tra.erase(find(clu_tra.begin(), clu_tra.end(), *clst_itr));
					impl_circuit->Merge(tar, *clst_itr);
				}
				it = impl_circuit->Merge(tar, seed);
				//remove the nodes in fanin cone which is not used any more
				for (Node* nd : seed->nodes_c[0]->inputs_pre) {
					if (nd->outputs.empty() && !nd->is_input && !nd->is_output)
						impl_circuit->Remove(nd);
				}
			}
			else {
				impl_circuit->Restore(seed->nodes_c);
				it++;
				continue;
			}
		}
		else
			it++;
	}
}

void NodeMerge::FaninMerge() {
	ClusterUnset::iterator it = impl_circuit->all_clusters.begin();
	fanin_cnt = 0;
	while(it != impl_circuit->all_clusters.end()){
	cout << "=================================================" << endl;
		Cluster* seed = *it;
		NodeSet fanin_all;
		NodeSet fanin_new;
		cout << "seed : " << seed->name << endl;//

		for (Node* nd : seed->input_nodes) {
			cout << "input " << nd->name << " :" << endl;//
			if (nd->is_input) {
				fanin_all.insert(nd);
				continue;
			}
			cout << "   ";//
			for (Node * in : nd->my_cluster->input_nodes) {
				cout << in->name << ", ";//
				fanin_all.insert(in);
				fanin_new.insert(in);
			}
			cout << endl;
			if (fanin_all.size() > limit_in)
				break;
		}
		if (fanin_all.size() <= limit_in && fanin_new.size() != 0) {
			NodeSet inputs = seed->input_nodes;
			seed->input_nodes.clear();
			for (Node* nd : inputs) {
				if (nd->is_input){
					seed->input_nodes.insert(nd);
					continue;
				}
				if (seed == nd->my_cluster)
					continue;
				impl_circuit->Merge(seed, nd->my_cluster);
			}
			fanin_cnt++;
		/*	cout << "fanin_all: " << endl;
			for (Node* nd : fanin_all)
				cout<<nd->name << ", ";
			cout << endl;
			cout<<"seed->input: "<<endl;
			for (Node* nd : seed->input_nodes)
				cout <<nd->name << ", ";
			cout << endl;*/
		}
		it++;
	}
}

void NodeMerge::QuickMerge() {
	quick_cnt = 0;
	for (Cluster* seed : impl_circuit->all_clusters) {
		while (seed->input_nodes.size() < limit_in) {//
			Cluster* selected = NULL;
			int min = limit_in + 1;//
			ClusterVector cand_c;
			ClusterSet c_tfo;
			ClusterSet c_tfi;
			impl_circuit->GetFanoutCone(seed, c_tfo);
			impl_circuit->GetFaninCone(seed, c_tfi);
			for (Cluster* clst : impl_circuit->all_clusters)
				if (c_tfo.find(clst) == c_tfo.end() && c_tfi.find(clst) == c_tfi.end() && clst!= seed)
					cand_c.push_back(clst);
			for (int j = 0; j < cand_c.size(); j++) {
				NodeSet inputs;
				inputs.insert(seed->input_nodes.begin(),seed->input_nodes.end());
				inputs.insert(cand_c.at(j)->input_nodes.begin(), cand_c.at(j)->input_nodes.end());
				if (inputs.size() < min) {
					selected = cand_c.at(j);
					min = inputs.size();
				}
			}
			if (selected != NULL) {
				impl_circuit->Merge(seed , selected);
				quick_cnt++;
			}
			else
				break;
		}
	}
}
//initialize the candidates
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
				&& find(nodes_nc.begin(), nodes_nc.end(), TFI_node) != nodes_nc.end() && TFI_node!=seed) {//if not in TFO of seed && not PI && in nodes_c
				cands_set.insert(TFI_node);
				cand_inputs_set.insert(TFI_node->inputs.begin(), TFI_node->inputs.end());
			}
		}
	}
	//convert to vector
	cands.insert(cands.begin(), cands_set.begin(), cands_set.end());
	cand_inputs.insert(cand_inputs.begin(), cand_inputs_set.begin(), cand_inputs_set.end());
	//show the info
	//cout << "cands: " << cands.size() << endl;
	//cout << "cand_inputs: " << cand_inputs.size() << endl;
}

//cluster version
void NodeMerge::InitCand(Cluster* seed, ClusterVector &cand_clus, NodeVector& cand_inputs) {
	cand_clus.clear();
	cand_inputs.clear();

	NodeSet n_tfo_of_seed, n_po_of_seed, n_cands, n_candin;
	ClusterSet c_tfo_of_seed, c_cands;

	// get the nodes in seed's fanout cone and PO
	for (Node* n_seed : seed->nodes_c) {
		NodeSet tfo, po;
		impl_circuit->GetFanoutCone(n_seed, tfo, po);
		n_tfo_of_seed.insert(tfo.begin(), tfo.end());
		n_po_of_seed.insert(po.begin(), po.end());
	}

	// get the clusters in seed's fanout cone
	impl_circuit->GetFanoutCone(seed, c_tfo_of_seed);

	// get the candidate nodes
	for (Node* n_out : n_po_of_seed) {
		Node* n_out_s = spec_circuit->GetNode(n_out->name);
		NodeSet tmp, pi_s;
		spec_circuit->GetFaninCone(n_out_s, tmp, pi_s);
		for (Node* n_pi_s : pi_s) {
			Node* n_pi = impl_circuit->GetNode(n_pi_s->name);
			NodeSet n_tfo_of_pi, tmp;
			impl_circuit->GetFanoutCone(n_pi, n_tfo_of_pi, tmp);
			for (Node* n_t : n_tfo_of_pi)
				if (n_tfo_of_seed.find(n_t) == n_tfo_of_seed.end())
					n_cands.insert(n_t);
		}
	}
	// erase the clsts in fanout cone of seed
	for (Node* cand_node : n_cands) {
		if (c_tfo_of_seed.find(cand_node->my_cluster) == c_tfo_of_seed.end() && cand_node->my_cluster != seed) {
			c_cands.insert(cand_node->my_cluster);
			n_candin.insert(cand_node->my_cluster->input_nodes.begin(), cand_node->my_cluster->input_nodes.end());
		}
	}
	//shrink the PO;
/*	impl_circuit->outputs.clear();
	impl_circuit->outputs.insert(impl_circuit->outputs.begin(), n_po_of_seed.begin(), n_po_of_seed.end());*/
	//convert to vector
	cand_clus.reserve(c_cands.size());
	cand_inputs.reserve(n_candin.size());
	cand_clus.insert(cand_clus.end(), c_cands.begin(), c_cands.end());
	cand_inputs.insert(cand_inputs.end(), n_candin.begin(), n_candin.end());

	//show the info
	cout << seed->name << " : ";
	for (Node* node : seed->nodes_c) {
		cout << node->name << "(" << node->order <<"), ";
	}
	cout << endl;
	cout << "candidate clusters: " << cand_clus.size() << endl;
	cout << "candidate input nodes: " << cand_inputs.size() << endl;
}

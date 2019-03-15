#include <iostream>
#include "restructure_c.h"
#include "faninselection_c.h"

using namespace std;

ClusterRestructurer::ClusterRestructurer(nodecircuit::Circuit* impl_circuit, nodecircuit::Circuit* spec_circuit) {
	this->impl_circuit = impl_circuit;
	this->spec_circuit = spec_circuit;
}

void ClusterRestructurer::SortByOrder(){

}

void ClusterRestructurer::Run() {
	for (auto cluster : impl_circuit->all_clusters) {
		RunOnce(cluster);
	}
}

void ClusterRestructurer::RunOnce(nodecircuit::Cluster* cluster) {
	times++;
	if (times % 10 == 1)
		std::cout << "\nthe " << times << "st iteration" << std::endl;
	else if (times % 10 == 2)
		std::cout << "\nthe " << times << "nd iteration" << std::endl;
	else if (times & 10 == 3)
		std::cout << "\nthe " << times << "rd iteration" << std::endl;
	else
		std::cout << "\nthe " << times << "th iterarion" << std::endl;

	int size_inputs_ori = cluster->input_nodes.size(), size_inputs_opt = 99;
	
	std::cout << "processing targets: ";
	for (auto node : cluster->nodes_c) {
		std::cout<< " "<<node->name;
	}
	std::cout << std::endl;

	nodecircuit::NodeVector cands;
	unsigned long num_cands;
	CreateInitialCandidates(cluster->nodes_c, cands, num_cands);

	nodecircuit::NodeVector solution_nodes;

	FaninSelectorC fanin_selector(spec_circuit, impl_circuit, &cands, &(cluster->nodes_c), &solution_nodes, iteration_limit);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "bfparser.h"
#include "node.h"

#include "shape_verilog.h"
#include "patchgen.h"
#include "getDC.h"
#include "CNFGenerator.h"
#include "mincut.h"
#include "nodemerge.h"

using namespace std;
using namespace nodecircuit;
using namespace veriloggateparser;

#define MAX_INITIAL_ITERATION_LIMIT 1023// 256+127
#define MAX_INTERMEDIATE_ITERATION_LIMIT 63
#define MAX_ITERATION_LIMIT 1023

//#define GLOBAL_TIMEOUT  1800
// returns the elapsed time from the beginning in seconds!
static clock_t g_start_time;
int GlobalElapsedTime() {
	return ((float)(clock() - g_start_time)) / CLOCKS_PER_SEC;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////        ¨€¨€¨€¨[   ¨€¨€¨€¨[ ¨€¨€¨€¨€¨€¨[ ¨€¨€¨[¨€¨€¨€¨[   ¨€¨€¨[            ////
////        ¨€¨€¨€¨€¨[ ¨€¨€¨€¨€¨U¨€¨€¨X¨T¨T¨€¨€¨[¨€¨€¨U¨€¨€¨€¨€¨[  ¨€¨€¨U            ////
////        ¨€¨€¨X¨€¨€¨€¨€¨X¨€¨€¨U¨€¨€¨€¨€¨€¨€¨€¨U¨€¨€¨U¨€¨€¨X¨€¨€¨[ ¨€¨€¨U            ////
////        ¨€¨€¨U¨^¨€¨€¨X¨a¨€¨€¨U¨€¨€¨X¨T¨T¨€¨€¨U¨€¨€¨U¨€¨€¨U¨^¨€¨€¨[¨€¨€¨U            ////
////        ¨€¨€¨U ¨^¨T¨a ¨€¨€¨U¨€¨€¨U  ¨€¨€¨U¨€¨€¨U¨€¨€¨U ¨^¨€¨€¨€¨€¨U            ////
////        ¨^¨T¨a     ¨^¨T¨a¨^¨T¨a  ¨^¨T¨a¨^¨T¨a¨^¨T¨a  ¨^¨T¨T¨T¨a            ////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
int main(int argc, char*argv[])
{
	g_start_time = clock();
	if (argc < 3) {
		cout << "usage: resyn inputfile outputfile  num (-n targets)" << endl;
		return 0;
	}

	string spec_filename(argv[1]);
	string impl_filename(argv[1]);
	string output_filename(argv[2]);
	//int num(atoi(argv[4]));
	int time_limit;
	int iteration_limit;
	vector<string> targetNamesToChange;
	int ch;
	while ((ch = getopt(argc, argv, "t:i:n:")) != -1) {
		switch (ch) {
		case 't': time_limit = atoi(optarg);
		case 'i': iteration_limit = atoi(optarg);
		case 'n': 
			targetNamesToChange.push_back(string(optarg));
			if (optind < argc) {
				targetNamesToChange.push_back(string(argv[optind]));
				optind++;
			}
		}
	}

	FILE* spec_file = fopen(spec_filename.c_str(), "r");
	if (spec_file == NULL) {
		cout << "cannot open spec file: " << spec_filename << endl;
		return 0;
	}

	FILE* impl_file = fopen(impl_filename.c_str(), "r");
	if (impl_file == NULL) {
		cout << "cannot open impl file: " << spec_filename << endl;
		return 0;
	}

	// circuits related to specification and implementation
	Circuit spec_circuit;
	Circuit impl_circuit;
	
	// now parse the specification and implementation files
	ofstream parser_log("parser.log");

	cout << "start reading the file" << endl;
	Lexer spec_parser(spec_file, parser_log);
	spec_parser.circuit = &spec_circuit;
	spec_parser.lex();

	Lexer impl_parser(impl_file, parser_log);
	impl_parser.circuit = &impl_circuit;
	impl_parser.lex();

	fclose(spec_file);
	//fclose(impl_file);

	cout << "PO: " << impl_circuit.outputs.size()<< endl;
	cout << "PI: " << impl_circuit.inputs.size() << endl;
	impl_circuit.WriteBlif("test.blif");
	//return 0;

	// add dummy buffers to consider primary inputs
	/*for (int impl_pi_cnt = 0; impl_pi_cnt < impl_circuit.inputs.size(); impl_pi_cnt++) {
		Node* pi_node = impl_circuit.inputs[impl_pi_cnt];
		Node* dummy = new Node;
		dummy->cube = "1 1\n";
		dummy->name = pi_node->name + "dummy";
		dummy->inputs.push_back(pi_node);
		pi_node->outputs.push_back(dummy);
		impl_circuit.all_nodes.push_back(dummy);
		impl_circuit.all_nodes_map[dummy->name] = dummy;
	}*/

	////////////////////////////////////////
	//////////    LETS DEBUG !    //////////
	////////////////////////////////////////

	//try to traverse all the combinations of internal nodes
	//NodeRestructurer impl_reselector(&impl_circuit, &spec_circuit);
	impl_circuit.AssignOrder();
	impl_circuit.SortByOrder();
	//impl_circuit.GetCluster(2, 5, false);

	//if(targetNamesToChange.empty())
	//	impl_reselector.Traverse(num);
	//else {
	//	NodeVector targets_to_change;
	//	for (string target_name : targetNamesToChange) {
	//		targets_to_change.push_back(impl_circuit.GetNode(target_name));
	//	}
	//	//impl_iterator.RunOnce(targets_to_change);
	//	impl_reselector.Traverse(num, targets_to_change);
	//}
	cout << "cnm" << endl;
	impl_circuit.InitCluster();
	cout << "cnm" << endl;
	int num_clst_ori=impl_circuit.all_clusters.size();
	int num_node_ori=impl_circuit.all_nodes.size();

	NodeMerge impl_merge(&impl_circuit, &spec_circuit);
	cout<<"==============select one============= " << endl;
	impl_merge.SelectMerge(1);
	int one_cnt1 = impl_merge.one_cnt;
	cout << "==============fanin merge===========" << endl;
	impl_merge.FaninMerge();
	int fanin_cnt1 = impl_merge.fanin_cnt;
	cout<<"==============select two============= " << endl;
	impl_merge.SelectMerge(2);
	int one_cnt2 = impl_merge.one_cnt;
	int two_cnt2 = impl_merge.two_cnt;
	cout<<"==============fanin merge============= " << endl;
	impl_merge.FaninMerge();
	int fanin_cnt2 = impl_merge.fanin_cnt;
	cout << "==============quick merge============= " << endl;
	impl_merge.QuickMerge();
	cout << "====================================== " << endl;

	cout << "num of nodes before: " << num_node_ori << endl;
	cout << "num of nodes now: " << impl_circuit.all_nodes.size() << endl;
	cout << "num of clusters before: " << num_clst_ori << endl;
	cout << "num of clusters now: " << impl_circuit.all_clusters.size()<< endl;
	int cnt1;
	int cnt2;
	for (Node* node : impl_circuit.all_nodes) {
		if (node->is_input || node->is_output)
			continue;
		else if (!node->HasOutput()) {
			cnt1++;
			if (node->my_cluster->nodes_c.size() != 1)
				cnt2++;
		}
	}
	cout << cnt1 << " nodes have no output" << endl;
	cout << cnt2 << " of them is merged with other nodes" << endl;
	cout << "merged with one cluster(first time): " << one_cnt1 << endl;
	cout << "fanin merging 1: " << fanin_cnt1 << endl;
	cout << "merged with one cluster(second time): " << one_cnt2 << endl;
	cout << "merged with two clusters(second time): " << two_cnt2 << endl;
	//cout << "merged with three clusters: " << impl_merge.three_cnt << endl;
	cout << "fanin merging 2: " << fanin_cnt2 << endl;
	cout << "quick merging: " << impl_merge.quick_cnt << endl;
	for (Cluster* clst : impl_circuit.all_clusters) {
		cout << clst->nodes_c.size() << "[" << clst->input_nodes.size() << "]";
	}
	cout << endl;
	int io_cnt[10][100];
	for(int i = 0; i < 10; i++) {
		for (int j = 0; j < 100; j++)
			io_cnt[i][j] = 0;
	}
	for (Cluster* clst : impl_circuit.all_clusters) {
		int in = clst->input_nodes.size();
		int out = 0;
		for (Node* nd : clst->nodes_c) {
			if (nd->is_output)
				out++;
			for (Node* output : nd->outputs) {
				if (find(clst->nodes_c.begin(), clst->nodes_c.end(), output) == clst->nodes_c.end()) {
					out++;
					break;
				}
			}
		}
		io_cnt[in][out]++;
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 100; j++) {
			if(io_cnt[i][j]!=0)
				cout << i << "-input-" << j << "-output: " << io_cnt[i][j] << endl;
		}
	}
	cout << "Elapsed time: " << GlobalElapsedTime() << endl;


//remove dummy buffers
/*	for (Node* pi_node : impl_circuit.inputs) {
		string dummy_name = pi_node->name + "dummy";
		Node* dummy = impl_circuit.all_nodes_map[dummy_name];
		pi_node->outputs.erase(find(pi_node->outputs.begin(), pi_node->outputs.end(), dummy));
		for (Node* output_node : dummy->outputs) {
			auto input_node_it = find(output_node->inputs.begin(), output_node->inputs.end(), dummy);
			*input_node_it = pi_node;
		}
		impl_circuit.all_nodes_map.erase(dummy_name);
		impl_circuit.all_nodes.erase(find(impl_circuit.all_nodes.begin(), impl_circuit.all_nodes.end(), dummy));
		delete dummy;
	}*/
	string output_filename_m = "m_" + output_filename;
	impl_circuit.WriteBlif_m(output_filename_m);
	impl_circuit.WriteBlif(output_filename);
	return 0;
}

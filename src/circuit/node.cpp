#include "node.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cassert>

using namespace std;

namespace nodecircuit {

	int Circuit::PurgeCircuit() {
		NodeVector node_process_list;
		NodeSet node_purge_set;
		for (int node_cnt = 0; node_cnt < all_nodes.size(); node_cnt++) {
			Node* node = all_nodes[node_cnt];
			if (node->outputs.size() == 0 && !node->is_output)
				node_process_list.push_back(node);
		}
		while (node_process_list.size() > 0) {
			Node* node = node_process_list.back();
			node_process_list.pop_back();
			node_purge_set.insert(node);
			if (!node->is_input) {
				for (int in_cnt = 0; in_cnt < node->inputs.size(); in_cnt++) {
					Node* in_node = node->inputs[in_cnt];
					for (NodeVector::iterator node_it = in_node->outputs.begin(); node_it != in_node->outputs.end(); ++node_it) {
						if (*node_it == node) {
							in_node->outputs.erase(node_it);
							break;
						}
					}
					if (!in_node->is_output && in_node->outputs.size() == 0)
						node_process_list.push_back(in_node);
				}
			}
		}
		if (all_nodes_map.find("zero") != all_nodes_map.end()) {
			Node* node = all_nodes_map["zero"];
			if (node->outputs.size() == 0)
				node_purge_set.insert(node);
		}
		if (all_nodes_map.find("one") != all_nodes_map.end()) {
			Node* node = all_nodes_map["one"];
			if (node->outputs.size() == 0)
				node_purge_set.insert(node);
		}
		if (node_purge_set.size() > 0) {
			int cur_ptr, move_ptr;
			cur_ptr = 0;
			while (node_purge_set.find(all_nodes[cur_ptr]) == node_purge_set.end())
				cur_ptr++;
			move_ptr = cur_ptr + 1;
			while (node_purge_set.find(all_nodes[move_ptr]) != node_purge_set.end())
				move_ptr++;
			while (move_ptr < all_nodes_map.size()) {
				if (node_purge_set.find(all_nodes[move_ptr]) == node_purge_set.end()) {
					all_nodes[cur_ptr] = all_nodes[move_ptr];
					cur_ptr++;
					move_ptr++;
				}
				else {
					move_ptr++;
				}
			}
			all_nodes.resize(cur_ptr);
		}

		return node_purge_set.size();
	}

	//int Circuit::WriteVerilog(string filename) {
	//	ofstream out_file_stream(filename);
	//	if (!out_file_stream.is_open())
	//		return 0;

	//	int i, j, k;

	//	out_file_stream << "module " << name << " (" << endl;
	//	for (i = 0; i < outputs.size(); i++)
	//		out_file_stream << "  " << outputs[i]->name << "," << endl;
	//	for (i = 0; i < inputs.size() - 1; i++)
	//		out_file_stream << "  " << inputs[i]->name << "," << endl;
	//	out_file_stream << "  " << inputs[i]->name << ");" << endl;


	//	out_file_stream << "input" << endl;
	//	for (i = 0; i < inputs.size() - 1; i++)
	//		out_file_stream << "  " << inputs[i]->name << "," << endl;
	//	out_file_stream << "  " << inputs[i]->name << ";" << endl;

	//	out_file_stream << "output" << endl;
	//	for (i = 0; i < outputs.size() - 1; i++)
	//		out_file_stream << "  " << outputs[i]->name << "," << endl;
	//	out_file_stream << "  " << outputs[i]->name << ";" << endl;

	//	int all_nodes_size_minus = all_nodes.size() - 1;
	//	while (all_nodes[all_nodes_size_minus]->is_input || all_nodes[all_nodes_size_minus]->is_output) {
	//		if (--all_nodes_size_minus < 0) {
	//			break;
	//		}
	//	}
	//	if (all_nodes_size_minus >= 0) {
	//		out_file_stream << "wire" << endl;
	//		if (all_nodes_size_minus > 0) {
	//			for (i = 0; i < all_nodes_size_minus; i++) {
	//				Node *node = all_nodes[i];
	//				if (!node->is_input && !node->is_output) {
	//					out_file_stream << "  " << all_nodes[i]->name << "," << endl;
	//				}
	//			}
	//		}
	//		out_file_stream << "  " << all_nodes[i]->name << ";" << endl;
	//	}

	//	// fix the name of zero and one nodes, if they exist
	//	if (all_nodes_map.find("zero") != all_nodes_map.end())
	//		all_nodes_map["zero"]->name = "1'b0";
	//	if (all_nodes_map.find("one") != all_nodes_map.end())
	//		all_nodes_map["one"]->name = "1'b1";

	//	for (i = 0; i < all_nodes.size(); i++) {
	//		Node* node = all_nodes[i];
	//		if (node->is_input)
	//			continue;
	//		if (node->inputs.size() == 0)
	//			continue;
	//		switch (node->type) {
	//		case NODE_BUF:
	//			out_file_stream << "  buf (";
	//			break;
	//		case NODE_NOT:
	//			out_file_stream << "  not (";
	//			break;
	//		case NODE_AND:
	//			out_file_stream << "  and (";
	//			break;
	//		case NODE_NAND:
	//			out_file_stream << "  nand (";
	//			break;
	//		case NODE_OR:
	//			out_file_stream << "  or (";
	//			break;
	//		case NODE_NOR:
	//			out_file_stream << "  nor (";
	//			break;
	//		case NODE_XOR:
	//			out_file_stream << "  xor (";
	//			break;
	//		case NODE_XNOR:
	//			out_file_stream << "  xnor (";
	//			break;
	//		default:
	//			out_file_stream << "buff (";
	//			// this case should not happen except for target nodes
	//			// TODO: check why this case has happened
	//		}
	//		out_file_stream << " " << node->name;
	//		for (j = 0; j < node->inputs.size(); j++)
	//			out_file_stream << " , " << node->inputs[j]->name;
	//		out_file_stream << " );" << endl;
	//	}
	//	out_file_stream << "endmodule" << endl;

	//	out_file_stream.close();

	//	// fix the name of zero and one nodes, if they exist
	//	if (all_nodes_map.find("zero") != all_nodes_map.end())
	//		all_nodes_map["zero"]->name = "zero";
	//	if (all_nodes_map.find("one") != all_nodes_map.end())
	//		all_nodes_map["one"]->name = "one";

	//	return 0;
	//}

	int Circuit::WriteBlif(string filename) {
		ofstream out_file_stream(filename);
		if (!out_file_stream.is_open())
			return 0;

		int i, j, k;

		out_file_stream << ".model " << name << endl;
		out_file_stream << ".inputs ";
		for (i = 0; i < inputs.size(); i++)
			out_file_stream << " \\" << endl << " " << inputs[i]->name;
		out_file_stream << endl;
		out_file_stream << ".outputs ";
		for (i = 0; i < outputs.size(); i++)
			out_file_stream << " \\" << endl << " " << outputs[i]->name;
		out_file_stream << endl;
		// write the name of zero and one nodes, if they exist
		if (all_nodes_map.find("zero") != all_nodes_map.end())
			out_file_stream << ".names zero" << endl << "0" << endl;
		if (all_nodes_map.find("one") != all_nodes_map.end())
			out_file_stream << ".names one" << endl << "1" << endl;
		for (i = 0; i < all_nodes.size(); i++) {
			Node* node = all_nodes[i];
			if (node->is_input)
				continue;
			if (node->name == "one" || node->name == "zero")
				continue;
			out_file_stream << ".names";
			for (j = 0; j < node->inputs.size(); j++)
				out_file_stream << " " << node->inputs[j]->name;
			out_file_stream << " " << node->name << endl;
			out_file_stream << node->cube << endl;
		}
		out_file_stream << ".end" << endl;
		out_file_stream << endl;

		out_file_stream.close();
		return 0;
	}

	int Circuit::ReadWeights(string filename) {
		ifstream in_weight_file(filename);
		if (!in_weight_file.is_open())
			return 0;

		string signal_name;
		int weight;

		char c;
		in_weight_file.get(c);
		while (in_weight_file.good()) {
			signal_name.clear();
			while (c != ' ' && c != '\t') {
				signal_name.push_back(c);
				in_weight_file.get(c);
			}
			while (c == ' ' || c == '\t') {
				in_weight_file.get(c);
			}
			weight = 0;
			while (c != '\n' && c != '\r' && c != EOF) {
				weight = weight * 10 + (c - '0');
				in_weight_file.get(c);
			}
			while ((c == '\n' || c == '\r') && in_weight_file.good()) {
				in_weight_file.get(c);
			}

			// cout << signal_name << " -> " << weight << endl;
			all_nodes_map[signal_name]->weight = weight;
		}


		in_weight_file.close();
		return 0;
	}

	int Circuit::AssignOrder() {
		bool flag = true;
		while (flag) {
			flag = false;
			for (auto node : all_nodes) {
				if (!node->is_input) {
					for (auto input : node->inputs) {
						if (input->order <= node->order) {
							input->order = node->order + 1;
							flag = true;
						}
					}
				}
			}
		}
	}

	bool CompareByOrder(Node* node1, Node* node2) {
		return node1->order < node2->order;
	}

	bool CompareByFanout(Node* node1, Node* node2) {
		return node1->outputs.size() < node2->outputs.size();
	}

	int Circuit::SortByOrder() {
		sort(all_nodes.begin(), all_nodes.end(), CompareByOrder);
	}
	
	int Circuit::SortByFanout() {
		sort(all_nodes.begin(), all_nodes.end(), CompareByFanout);
	}

	int Circuit::GetFaninCone(Node* node, NodeSet& fanin_nodes, NodeSet& fanin_pi) {
		NodeVector not_processed;
		not_processed.push_back(node);
		while (not_processed.size() > 0) {
			Node* current_node = not_processed.back();
			not_processed.pop_back();
			if (current_node->is_input) {
				fanin_pi.insert(current_node);
			}
			else { // see the Note below
				fanin_nodes.insert(current_node);
				for (int i = 0; i < current_node->inputs.size(); i++)
					if (fanin_nodes.find(current_node->inputs[i]) == fanin_nodes.end())
						not_processed.push_back(current_node->inputs[i]);
			}
		}
		fanin_nodes.erase(node);

		// Note: fanin_nodes does not contain fanin_pi
		//       comment the else part so that fanin_nodes does contain fanin_pi
		return 0;
	}

	int Circuit::GetFanoutCone(Node* node, NodeSet& fanout_nodes, NodeSet& fanout_po) {
		NodeVector not_processed;
		not_processed.push_back(node);
		while (not_processed.size() > 0) {
			Node* current_node = not_processed.back();
			not_processed.pop_back();
			if (current_node->is_output) {
				fanout_po.insert(current_node);
			}
			else { // -> see the Note below
			fanout_nodes.insert(current_node);
			for (int i = 0; i < current_node->outputs.size(); i++)
				if (fanout_nodes.find(current_node->outputs[i]) == fanout_nodes.end())
					not_processed.push_back(current_node->outputs[i]);
			}
		}
		fanout_nodes.erase(node);

		// Note: fanout_nodes do contain fanout_po
		//       uncomment the else part so that fanout_nodes does not contain fanout_po
		return 0;
	}

	int Circuit::GetFanoutCone(NodeSet& input_nodes, NodeSet& fanout_nodes, NodeSet& exclude_nodes) {
		NodeVector not_processed;
		for (NodeSet::iterator in_it = input_nodes.begin(); in_it != input_nodes.end(); ++in_it)
			not_processed.push_back(*in_it);
		while (not_processed.size() > 0) {
			Node* current_node = not_processed.back();
			not_processed.pop_back();
			if (exclude_nodes.find(current_node) == exclude_nodes.end()) {
				fanout_nodes.insert(current_node);
				for (int i = 0; i < current_node->outputs.size(); i++)
					if (fanout_nodes.find(current_node->outputs[i]) == fanout_nodes.end())
						not_processed.push_back(current_node->outputs[i]);
			}
		}
		for (NodeSet::iterator in_it = input_nodes.begin(); in_it != input_nodes.end(); ++in_it)
			fanout_nodes.erase(*in_it);

		return 0;
	}

	int Circuit::SetTargets(std::vector<std::string> node_names) {
		for (auto node_name : node_names) {
			Node* node = this->GetNode(node_name);
			if (node->is_input || node->is_output) {
				std::cout << " PI/PO cannot be targets" << std::endl;
				return 0;
			}
			for (auto input : node->inputs) {
				auto it = std::find(input->outputs.begin(), input->outputs.end(), node);
				assert(it != input->outputs.end());
				input->outputs.erase(it);
			}
			targets.push_back(node);
			node->inputs_pre = node->inputs;
			node->inputs.clear();
			node->cube_pre = node->cube;
			node->cube.clear();
		}
		return 0;
	}

	int Circuit::SetTargets(NodeVector nodes) {
		for (auto node : nodes) {
			std::cout << "setting " << node->name << " to target" << std::endl;
			if (node->is_input || node->is_output) {
				std::cout << " PI/PO cannot be targets" << std::endl;
				return 0;
			}
			for (auto input : node->inputs) {
				auto it = std::find(input->outputs.begin(), input->outputs.end(), node);
				assert(it != input->outputs.end());
				input->outputs.erase(it);
			}
			targets.push_back(node);
			node->inputs_pre = node->inputs;
			node->inputs.clear();
			node->cube_pre = node->cube;
			node->cube.clear();
		}
		return 0;
	}


	int Circuit::SelectTargets(std::vector<std::string>& node_names, Node * node, int n)
	{
		NodeSet fanin_set;
		NodeVector fanin_cand;
		NodeVector not_processed;
		NodeVector fanin_selected;
		not_processed.push_back(node);
		//get the fanin of node
		while (!not_processed.empty()) {
			Node* current_node = not_processed.back();
			not_processed.pop_back();
			if (!current_node->is_input && !current_node->is_output)
				fanin_set.insert(current_node);
			for (auto input : current_node->inputs)
				if (fanin_set.find(input) == fanin_set.end())
					not_processed.push_back(input);
		}
		fanin_set.erase(node);
		std::cout << fanin_set.size() << std::endl;
		if (fanin_set.size() == 0)
			return 0;
		for (auto node : fanin_set)
			fanin_cand.push_back(node);
		//std::srand((unsigned)time(NULL));
		for (NodeVector::iterator it = fanin_cand.begin(); it != fanin_cand.end();) {
			if ((*it)->inputs.size() == 1 && (*it)->inputs[0]->is_input) {
				it == fanin_cand.erase(it);
			}
			else
				it++;
		}

		while (fanin_selected.size() < n && fanin_cand.size() + fanin_selected.size() >= n) {
			bool will_delete_fanin = false;
			int num = std::rand() % fanin_cand.size();
			for (NodeVector::iterator it = fanin_selected.begin(); it != fanin_selected.end();it++) {
				Node* fanin_cand_cur = fanin_cand.at(num);
				while (fanin_cand_cur->outputs.size() == 1 && fanin_cand_cur->outputs[0] != node) {
					if (fanin_cand_cur->outputs[0] == *it) {
						will_delete_fanin = true;
						break;
					}
					else
						fanin_cand_cur = fanin_cand_cur->outputs[0];
				}
				if (will_delete_fanin)
					break;
				Node* fanin_selected_cur = *it;
				while (fanin_selected_cur->outputs.size() == 1 && fanin_selected_cur->outputs[0] != node) {
					if (fanin_selected_cur->outputs[0] == fanin_cand.at(num)) {
						it = fanin_selected.erase(it);
						it--;
						break;
					}
					else
						fanin_selected_cur = fanin_selected_cur->outputs[0];
				}
			}
			if (!will_delete_fanin) {
				fanin_selected.push_back(fanin_cand.at(num));
				
			}
		fanin_cand.erase(fanin_cand.begin() + num);
		}
		for (auto fanin : fanin_selected) {
			node_names.push_back(fanin->name);
		}
		return 0;
	}

	int Circuit::Restruct(Node * target, std::vector<std::map<Node*, bool>> in2cases)
	{
		targets.erase(std::find(targets.begin(), targets.end(), target));
		target->cube.clear();
		for (auto pair : in2cases[0]) {
			pair.first->outputs.push_back(target);
			target->inputs.push_back(pair.first);
		}
		for (auto in2case : in2cases) {
			for (auto pair : in2case) {
				pair.second ? target->cube += "1" : target->cube += "0";
			}
			target->cube += " 1\n";
		}
		return 0;
	}

	int Circuit::Restruct(Node * target_impl) {
		int pos = 0;
		Node * target_spec = this->GetNode(target_impl->name);
		for (auto input_spec_old : target_spec->inputs) {
			auto it = std::find(input_spec_old->outputs.begin(), input_spec_old->outputs.end(), target_spec);
			assert(it != input_spec_old->outputs.end());
			input_spec_old->outputs.erase(it);
		}
		target_spec->inputs.clear();
		target_spec->cube.clear();
		for (auto input_impl : target_impl->inputs) {
			int pos = 0;
			string input_impl_name = input_impl->name;
			if ((pos = input_impl_name.find("dummy")) != -1)
				input_impl_name.erase(pos, 5);
			Node* input_spec_new = this->GetNode(input_impl_name);
			target_spec->inputs.push_back(input_spec_new);
			input_spec_new->outputs.push_back(target_spec);
		}
		target_spec->cube = target_impl->cube;
	}

	int Circuit::Restore(NodeVector& targets) {
		for (auto target : targets) {
			if (!target->inputs.empty()) {
				for (auto input : target->inputs) {
					input->outputs.pop_back();
				}
			}
		}
		for (auto target : targets) {
			target->cube = target->cube_pre;
			target->inputs.clear();
			target->inputs = target->inputs_pre;
			target->inputs_pre.clear();
			for (auto input : target->inputs) {
				input->outputs.push_back(target);
			}
		}
		this->targets.clear();
		return 0;
	}

	int Circuit::IsInFanoutCone(NodeVector nodes_old, NodeVector nodes_new) {
		NodeSet fanout_cone_total;
		for (auto node : nodes_old) {
			NodeSet fanout_cone, fanout_po;
			GetFanoutCone(node, fanout_cone, fanout_po);
			for (auto node_fanout : fanout_cone) {
				fanout_cone_total.insert(node_fanout);
			}
		}
		int cnt = 0;
		for (auto node : nodes_new) {
			if (find(fanout_cone_total.begin(), fanout_cone_total.end(), node) != fanout_cone_total.end())
				cnt++;
		}
		std::cout << cnt << " of " << nodes_new.size() << " selected inputs are in the fanout cone of original inputs." << std::endl;
	}

	int Circuit::MakeCluster(NodeVector nodes_to_cluster)
	{
		Cluster* new_cluster = new Cluster;
		all_clusters.push_back(new_cluster);
		for (Node* node : nodes_to_cluster) {
			new_cluster->append(node);
		}
		return 0;
	}

	int Circuit::GetCluster(int max, int thred, bool by_input) {
		NodeVector nodes_unclustered = all_nodes;
		while (!nodes_unclustered.empty()) {
			Cluster* new_cluster = new Cluster;
			new_cluster->append(nodes_unclustered.back());
			nodes_unclustered.pop_back();
			NodeVector::reverse_iterator it = nodes_unclustered.rbegin();
			while (it != nodes_unclustered.rend() && new_cluster->nodes_c.size()< max) {
				if (Sharedness(*it, new_cluster, by_input) > thred) {
					new_cluster->append(*it);
					nodes_unclustered.erase(--(it.base()));
				}
				else
					it++;
			}
		}
	}

	int Circuit::Sharedness(Node* node, Cluster* cluster, bool by_input) {
		int max_shr = 0;
		int cur_shr = 0;
		for (auto node_c : cluster->nodes_c) {
			if (by_input) {
				NodeVector input_vec;
				NodeSet input_set;
				input_vec.insert(input_vec.end(), node->outputs.begin(), node->outputs.end());
				input_vec.insert(input_vec.end(), node_c->outputs.begin(), node_c->outputs.end());
				input_set.insert(node->outputs.begin(), node->outputs.end());
				input_set.insert(node_c->outputs.begin(), node_c->outputs.end());
				cur_shr = input_vec.size() - input_set.size();
				if (cur_shr > max_shr)
					max_shr = cur_shr;
				cur_shr = 0;
			}
			else {
				NodeVector output_vec;
				NodeSet output_set;
				output_vec.insert(output_vec.end(), node->outputs.begin(), node->outputs.end());
				output_vec.insert(output_vec.end(), node_c->outputs.begin(), node_c->outputs.end());
				output_set.insert(node->outputs.begin(), node->outputs.end());
				output_set.insert(node_c->outputs.begin(), node_c->outputs.end());
				cur_shr = output_vec.size() - output_set.size();
				if (cur_shr > max_shr)
					max_shr = cur_shr;
				cur_shr = 0;
			}
		}
		return max_shr;
	}
} // namespace nodecircuit

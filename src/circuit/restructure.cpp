#include <iostream>
#include <algorithm>
#include <omp.h>
#include <assert.h>
#include "restructure.h"
#include "faninselection.h"
#include "getDC.h"


	NodeRestructurer::NodeRestructurer(nodecircuit::Circuit* impl_circuit, nodecircuit::Circuit* spec_circuit) {
		this->impl_circuit = impl_circuit;
		this->spec_circuit = spec_circuit;
		times = 0;
		for (auto node : impl_circuit->all_nodes) {
			if (node->is_input || node->is_output || (node->inputs.size() == 1 && node->inputs[0]->is_input))
				continue;
			all_nodes.push_back(node);
		}
		std::cout << "number of all the nodes: " << all_nodes.size() << std::endl;
	}

	//DFS to get all the possible combinations
	void NodeRestructurer::GetCombs(int num, int pos) {
		for (int i = pos; i < all_nodes.size(); i++) {
			comb.push_back(all_nodes.at(i));
			if (num == 1) {
				combs.push_back(comb);
				comb.clear();
			}
			else {
				GetCombs(num - 1, i + 1);
			}
		}
	}

	void NodeRestructurer::Traverse(int num) {
		this->num = num;
		GetCombs(num, 0);
		std::cout << "there are " << combs.size() << " combinations" << std::endl;
		for (auto comb_vec : combs) {
			if (InOneGroup(comb_vec))
				RunOnce(comb_vec);
		}
	}
	
	//--------------Traverse from initial targets----------------	
	void NodeRestructurer::Traverse(int num, nodecircuit::NodeVector initial_targets) {
		assert(initial_targets.size() == num);
		this->num = num;
		bool ignore = true;
		for (int i = 0; i < all_nodes.size(); i++) {
			if (all_nodes.at(i) != initial_targets.at(0) & ignore) {
				times+=(num-1);
				continue;
			}
			else {
				ignore = false;
				nodecircuit::NodeSet fanout_cone, fanout_po;
				impl_circuit->GetFanoutCone(all_nodes.at(i), fanout_cone, fanout_po);
				if (fanout_po.empty())
					continue;
				if (num == 1)
					RunOnce({ all_nodes.at(i) });
				else {
					nodecircuit::NodeVector selected_nodes;
					nodecircuit::NodeSet po_set, fanout_set;
					selected_nodes.push_back(all_nodes.at(i));
					impl_circuit->GetFanoutCone(all_nodes.at(i), fanout_set, po_set);
					Traverse(selected_nodes, po_set, fanout_set, (i+1), (num-1), initial_targets);
				}
			}
		}
	}

	void NodeRestructurer::Traverse(nodecircuit::NodeVector selected_nodes, nodecircuit::NodeSet po_set, nodecircuit::NodeSet fanout_set, int pos, int num, nodecircuit::NodeVector initial_targets){
		int pos_ini = selected_nodes.size();
		bool ignore = true;

		//backup the po set & fanout cone set
		nodecircuit::NodeVector po_vec_ori, fanout_vec_ori;
		po_vec_ori.reserve(po_set.size());
		fanout_vec_ori.reserve(fanout_set.size());
		po_vec_ori.assign(po_set.begin(), po_set.end());
		fanout_vec_ori.assign(fanout_set.begin(), fanout_set.end());

		for (int i = pos; i < all_nodes.size(); i++) {
			if (all_nodes.at(i) != initial_targets.at(pos_ini) & ignore) {
				times += (num-1);
				continue;
			}
			else {
				ignore = false;
				//restore the po_set & fanout_set
				po_set.clear();
				fanout_set.clear();
				for (auto po_ori : po_vec_ori)
					po_set.insert(po_ori);
				for (auto fanout_ori : fanout_vec_ori)
					fanout_set.insert(fanout_ori);
				//the last node 
				if (Judge(po_set, fanout_set, selected_nodes, all_nodes.at(i))) {
					nodecircuit::NodeSet po_set_new, fanout_set_new;
					impl_circuit->GetFanoutCone(all_nodes.at(i), fanout_set_new, po_set_new);
					//update the selected nodes
					selected_nodes.push_back(all_nodes.at(i));
					for (auto fanout_new : fanout_set_new)
						fanout_set.insert(fanout_new);
					for (auto po_new : po_set_new)
						po_set.insert(po_new);
					if (num == 1) {//the last node
						std::cout << "\n" << "The " << ++times << "th combination" << std::endl;
						RunOnce(selected_nodes);
					}
					else {//the last numth node
						Traverse(selected_nodes, po_set, fanout_set, (i+1), (num-1), initial_targets);
					}
					selected_nodes.pop_back();
				}
				else//not the node we want, discard it
					continue;
			}
		}
	}

	//------------Judge whether the target node should be selected or abandoned----------------
	bool NodeRestructurer::Judge(nodecircuit::NodeSet po_set, nodecircuit::NodeSet fanout_set, nodecircuit::NodeVector selected_nodes, nodecircuit::Node* target_node) {
		nodecircuit::NodeSet po_set_new, fanout_set_new, po_all;
		//if the new target_node is in the fanout cone of previous targets, discard it
		if (fanout_set.find(target_node) != fanout_set.end())
			return false;

		impl_circuit->GetFanoutCone(target_node, fanout_set_new, po_set_new);
		//if the new target node's fanout cone includes the previous targets, discard it
		for (auto node : selected_nodes) {
			if (fanout_set_new.find(node) != fanout_set_new.end()) {
				std::cout << "There's a loop!" << std::endl;
				return false;
			}
		}
		//if the new target's PO and the previous ones' PO have no overlap, discard the new one
		for (auto node : po_set)
			po_all.insert(node);
		for (auto node : po_set_new) {
			po_all.insert(node);
		}
		if (po_all.size() < (po_set.size() + po_set_new.size()))
			std::cout << "po_all: " << po_all.size() << ", po_old+new: " << po_set.size() + po_set_new.size() << std::endl;
		else {
			std::cout << "There's no PO share" << std::endl;
			return false;
		}
		return true;
	}

	bool NodeRestructurer::InOneGroup(nodecircuit::NodeVector targets) {
		impl_circuit->SetTargets(targets);
		std::vector<nodecircuit::NodeVector*> target_groups;
		//std::vector<nodecircuit::NodeSet*> targets_fanout_cone;
		std::vector<nodecircuit::NodeSet*> targets_fanout_po;
		for (auto target : targets) {
			nodecircuit::NodeVector* target_group = new nodecircuit::NodeVector;
			nodecircuit::NodeSet* fanout_cone = new nodecircuit::NodeSet;
			nodecircuit::NodeSet* fanout_po = new nodecircuit::NodeSet;
			target_group->push_back(target);
			std::cout << "*" << std::endl;
			impl_circuit->GetFanoutCone(target, *fanout_cone, *fanout_po);
			std::cout << "**" << std::endl;
			if (fanout_po->empty()) {
				std::cout << "one target is blocked by others" << std::endl;
				impl_circuit->Restore(targets);
				break;
			}
			auto fanout_po_it = targets_fanout_po.begin();
			auto target_it = target_groups.begin();
			for (; fanout_po_it != targets_fanout_po.end();) {
				nodecircuit::NodeSet po_set;
				nodecircuit::NodeVector po_vector;
				for (auto po_node : *fanout_po) {
					po_set.insert(po_node);
					po_vector.push_back(po_node);
				}
				for (auto po_node : **fanout_po_it) {
					po_set.insert(po_node);
					po_vector.push_back(po_node);
				}
				if (po_set.size() < po_vector.size()) {
					*fanout_po = po_set;
					target_group->insert(target_group->end(), (*target_it)->begin(), (*target_it)->end());
					delete *fanout_po_it;
					delete *target_it;
					fanout_po_it = targets_fanout_po.erase(fanout_po_it);
					target_it = target_groups.erase(target_it);
				}
				else {
					fanout_po_it++;
					target_it++;
				}
			}
			target_groups.push_back(target_group);
			targets_fanout_po.push_back(fanout_po);
			delete fanout_cone;
		}
		if (target_groups.size() == 1) {
			std::cout << "targets are in one group" << std::endl;
			impl_circuit->Restore(targets);
			return true;
		}
		else {
			std::cout << "targets are not in one group" << std::endl;
			impl_circuit->Restore(targets);
			return false;
		}
		
	}

	//------------Run once fanin selection for selected nodes-------------
	void NodeRestructurer::RunOnce(nodecircuit::NodeVector selected_nodes) {
		times++;
		if (times % 10 == 1)
			std::cout << "\nthe " << times << "st iteration" << std::endl;
		else if (times % 10 == 2)
			std::cout << "\nthe " << times << "nd iteration" << std::endl;
		else if (times & 10 == 3)
			std::cout << "\nthe " << times << "rd iteration" << std::endl;
		else
			std::cout << "\nthe " << times << "th iterarion" << std::endl;
			
		int size_inputs_ori = 0, size_inputs_opt = 0;
		for (auto target : selected_nodes)
			size_inputs_ori += pow(2, target->inputs.size());

		impl_circuit->SetTargets(selected_nodes);

		std::vector<nodecircuit::NodeSet*> dummy_targets_nodes;
		std::vector<nodecircuit::NodeSet*> dummy_targets_fanin_cone;
		std::vector<nodecircuit::NodeSet*> dummy_targets_fanin_cone_new;
		std::vector<nodecircuit::NodeSet*> dummy_targets_fanin_pi;
		std::vector<nodecircuit::NodeSet*> dummy_targets_fanout_cone;
		std::vector<nodecircuit::NodeSet*> dummy_targets_fanout_po;

		std::cout << "Group" << std::endl;
		Group(dummy_targets_nodes, dummy_targets_fanin_cone, dummy_targets_fanin_cone_new, dummy_targets_fanin_pi, dummy_targets_fanout_cone, dummy_targets_fanout_po);

		nodecircuit::NodeVector org_impl_outputs;
		nodecircuit::NodeVector org_spec_outputs;
		nodecircuit::NodeVector org_impl_targets;
		org_impl_outputs.reserve(impl_circuit->outputs.size());
		org_impl_targets.reserve(impl_circuit->targets.size());
		org_spec_outputs.reserve(spec_circuit->outputs.size());
		org_impl_outputs.assign(impl_circuit->outputs.begin(), impl_circuit->outputs.end());
		org_impl_targets.assign(impl_circuit->targets.begin(), impl_circuit->targets.end());
		org_spec_outputs.assign(spec_circuit->outputs.begin(), spec_circuit->outputs.end());

		std::cout << "Process" << std::endl;
		size_inputs_opt = Process(dummy_targets_nodes, dummy_targets_fanin_cone, dummy_targets_fanout_cone, dummy_targets_fanout_po);

		spec_circuit->outputs.clear();
		impl_circuit->outputs.clear();
		spec_circuit->outputs.assign(org_spec_outputs.begin(), org_spec_outputs.end());
		impl_circuit->outputs.assign(org_impl_outputs.begin(), org_impl_outputs.end());

		if (impl_circuit->targets.empty()) {
			std::cout << "size of the original LUTs: " << size_inputs_ori << "\n";
			std::cout << "size of the restructed LUTs: " << size_inputs_opt << std::endl;
			if (size_inputs_opt < size_inputs_ori) {
				std::cout << "successfully shrink the fanin" << std::endl;
				for (auto target : org_impl_targets) {
					impl_circuit->IsInFanoutCone(target->inputs_pre, target->inputs);
					spec_circuit->Restruct(target);
				}
				std::string filename = std::to_string(this->num) + "opt.blif";
				impl_circuit->WriteBlif(filename);
			}
			else {
				impl_circuit->Restore(org_impl_targets);
				if (size_inputs_opt > size_inputs_ori) {
					std::cout << "become worse" << std::endl;
				}
				else {
					std::cout << "no change" << std::endl;
				}
			}
		}
		else {
			impl_circuit->Restore(org_impl_targets);
		}
	}

	/*----------group the selected nodes in the terms of their POs-----------*/
	void NodeRestructurer::Group(std::vector<nodecircuit::NodeSet*> &dummy_targets_nodes, std::vector<nodecircuit::NodeSet*> &dummy_targets_fanin_cone_old,
		std::vector<nodecircuit::NodeSet*> &dummy_targets_fanin_cone_new, std::vector<nodecircuit::NodeSet*> &dummy_targets_fanin_pi,
		std::vector<nodecircuit::NodeSet*> &dummy_targets_fanout_cone, std::vector<nodecircuit::NodeSet*> &dummy_targets_fanout_po) {
		std::vector<nodecircuit::NodeSet*> targets_fanin_cone_new;
		std::vector<nodecircuit::NodeSet*> targets_fanin_cone_old;
		std::vector<nodecircuit::NodeSet*> targets_fanin_pi;
		std::vector<nodecircuit::NodeSet*> targets_fanout_cone;
		std::vector<nodecircuit::NodeSet*> targets_fanout_po;
		std::vector<nodecircuit::NodeSet*> targets_inputs;

		for (int target_cnt = 0; target_cnt < impl_circuit->targets.size(); target_cnt++) {
			nodecircuit::NodeSet *fanin_cone_old = new nodecircuit::NodeSet;
			nodecircuit::NodeSet *fanin_cone_new = new nodecircuit::NodeSet;
			nodecircuit::NodeSet *fanin_pi = new nodecircuit::NodeSet;
			nodecircuit::NodeSet *fanout_cone = new nodecircuit::NodeSet;
			nodecircuit::NodeSet *fanout_po = new nodecircuit::NodeSet;
			nodecircuit::NodeSet *inputs = new nodecircuit::NodeSet;
			targets_fanin_cone_old.push_back(fanin_cone_old);
			targets_fanin_cone_new.push_back(fanin_cone_new);
			targets_fanin_pi.push_back(fanin_pi);
			targets_fanout_cone.push_back(fanout_cone);
			targets_fanout_po.push_back(fanout_po);
			// TODO: delete these NodeSets before return!
			nodecircuit::Node *target_node = impl_circuit->targets[target_cnt];
			// find the fanout cone nodes and primary outputs of the target node
			impl_circuit->GetFanoutCone(target_node, *fanout_cone, *fanout_po);
			// find the common primary inputs for the same primary outputs in the spec circuit
			nodecircuit::NodeSet spec_common_fanin_pi, spec_current_fanin_pi;
			nodecircuit::NodeSet::iterator fanout_po_it = fanout_po->begin();
			do {
				spec_circuit->GetFaninPi(spec_circuit->GetNode((*fanout_po_it)->name), spec_common_fanin_pi);
				++fanout_po_it;
			} while (spec_common_fanin_pi.size() == 0); // exclude the constant functions
			while (fanout_po_it != fanout_po->end()) {
				spec_current_fanin_pi.clear();
				spec_circuit->GetFaninPi(spec_circuit->GetNode((*fanout_po_it)->name), spec_current_fanin_pi);
				if (spec_current_fanin_pi.size() > 0) { // exclude the constant functions!
					nodecircuit::NodeSet::iterator spec_fanin_it = spec_common_fanin_pi.begin();
					while (spec_fanin_it != spec_common_fanin_pi.end()) {
						if (spec_current_fanin_pi.find(*spec_fanin_it) != spec_current_fanin_pi.end())
							++spec_fanin_it;
						else
							spec_fanin_it = spec_common_fanin_pi.erase(spec_fanin_it);
					}
				}
				++fanout_po_it;
			}
			// find the corresponding primary inputs in the impl circuit
			for (nodecircuit::NodeSet::iterator spec_fanin_pi_it = spec_common_fanin_pi.begin();
				spec_fanin_pi_it != spec_common_fanin_pi.end(); ++spec_fanin_pi_it)
				fanin_pi->insert(impl_circuit->GetNode((*spec_fanin_pi_it)->name));

			// get the fanin cone for target according to fanout cone of its primary inputs, excluding the fanout cone nodes
			impl_circuit->GetFanoutCone(*fanin_pi, *fanin_cone_old, *fanout_cone);

			inputs->insert(target_node->inputs.begin(), target_node->inputs.end());
			impl_circuit->GetFanoutCone(*inputs, *fanin_cone_new, *fanout_cone);
			// write for debugging purpose!
			std::cout << "target: " << target_node->name << " -> (pi=" << fanin_pi->size() << ",fin_cone=" << fanin_cone_old->size() << "),(po=" << fanout_po->size() << ",fout_cone=" << fanout_cone->size() << ")" << std::endl;
			std::cout << "        pi: ";
			for (nodecircuit::NodeSet::iterator pi_it = fanin_pi->begin(); pi_it != fanin_pi->end(); ++pi_it)
				std::cout << (*pi_it)->name << " ";
			std::cout << std::endl;
			/*std::cout << "     fanin: ";
			for (nodecircuit::NodeSet::iterator fanin_it = fanin_cone->begin(); fanin_it != fanin_cone->end(); ++fanin_it)
				std::cout << (*fanin_it)->name << " ";
			std::cout << std::endl;*/
			std::cout << "        po: ";
			for (nodecircuit::NodeSet::iterator po_it = fanout_po->begin(); po_it != fanout_po->end(); ++po_it)
				std::cout << (*po_it)->name << " ";
			std::cout << std::endl;
			if (impl_circuit->targets.size() > 1) {
				std::cout << "        other target dependencies: ";
				for (int target_dep_cnt = 0; target_dep_cnt < impl_circuit->targets.size(); target_dep_cnt++)
					if (fanout_cone->find(impl_circuit->targets[target_dep_cnt]) != fanout_cone->end())
						std::cout << impl_circuit->targets[target_dep_cnt]->name << " ";
				std::cout << std::endl;
			}
		}



		std::cout << "new target group: 0 -> " << impl_circuit->targets[0]->name << std::endl;

		nodecircuit::NodeSet* targets_set = new nodecircuit::NodeSet;
		targets_set->insert(impl_circuit->targets[0]);
		dummy_targets_nodes.push_back(targets_set);

		bool empty = (targets_fanin_pi[0]->size() == 0) || (targets_fanin_cone_old[0]->size() == 0);

		nodecircuit::NodeSet* targets_po = new nodecircuit::NodeSet;
		if (!empty)
			targets_po->insert(targets_fanout_po[0]->begin(), targets_fanout_po[0]->end());
		dummy_targets_fanout_po.push_back(targets_po);

		nodecircuit::NodeSet* targets_pi = new nodecircuit::NodeSet;
		if (!empty)
			targets_pi->insert(targets_fanin_pi[0]->begin(), targets_fanin_pi[0]->end());
		dummy_targets_fanin_pi.push_back(targets_pi);

		nodecircuit::NodeSet* targets_fanin_old = new nodecircuit::NodeSet;
		if (!empty)
			targets_fanin_old->insert(targets_fanin_cone_old[0]->begin(), targets_fanin_cone_old[0]->end());

		nodecircuit::NodeSet* targets_fanin_new = new nodecircuit::NodeSet;
		if (!empty)
			targets_fanin_new->insert(targets_fanin_cone_new[0]->begin(), targets_fanin_cone_new[0]->end());

		dummy_targets_fanin_cone_old.push_back(targets_fanin_old);
		dummy_targets_fanin_cone_new.push_back(targets_fanin_new);

		//dummy_targets_fanin_cone.push_back(targets_fanin);

		nodecircuit::NodeSet* targets_fanout = new nodecircuit::NodeSet;
		if (!empty)
			targets_fanout->insert(targets_fanout_cone[0]->begin(), targets_fanout_cone[0]->end());
		dummy_targets_fanout_cone.push_back(targets_fanout);

		for (int t_cnt = 1; t_cnt < impl_circuit->targets.size(); t_cnt++) {
			nodecircuit::NodeSet test_node_set;
			empty = (targets_fanin_pi[t_cnt]->size() == 0) || (targets_fanin_cone_old[t_cnt]->size() == 0);
			int prev_added = -1;
			for (int d_cnt = 0; !empty && d_cnt < dummy_targets_nodes.size(); d_cnt++) {
				test_node_set.clear();
				test_node_set.insert(targets_fanout_po[t_cnt]->begin(), targets_fanout_po[t_cnt]->end());
				test_node_set.insert(dummy_targets_fanout_po[d_cnt]->begin(), dummy_targets_fanout_po[d_cnt]->end());
				if (test_node_set.size() < targets_fanout_po[t_cnt]->size() + dummy_targets_fanout_po[d_cnt]->size()) {
					if (prev_added < 0) {
						std::cout << "merge target group: " << d_cnt << " -> " << impl_circuit->targets[t_cnt]->name << std::endl;

						targets_set = dummy_targets_nodes[d_cnt];
						targets_set->insert(impl_circuit->targets[t_cnt]);

						targets_po = dummy_targets_fanout_po[d_cnt];
						targets_po->insert(targets_fanout_po[t_cnt]->begin(), targets_fanout_po[t_cnt]->end());

						targets_pi = dummy_targets_fanin_pi[d_cnt];
						targets_pi->insert(targets_fanin_pi[t_cnt]->begin(), targets_fanin_pi[t_cnt]->end());

						targets_fanout = dummy_targets_fanout_cone[d_cnt];
						targets_fanout->insert(targets_fanout_cone[t_cnt]->begin(), targets_fanout_cone[t_cnt]->end());

						targets_fanin_old = dummy_targets_fanin_cone_old[d_cnt];
						targets_fanin_old->insert(targets_fanin_cone_old[t_cnt]->begin(), targets_fanin_cone_old[t_cnt]->end());
						nodecircuit::NodeSet::iterator node_it = targets_fanin_old->begin();
						while (node_it != targets_fanin_old->end()) {
							if (targets_fanout->find(*node_it) != targets_fanout->end())
								node_it = targets_fanin_old->erase(node_it);
							else
								++node_it;
						}

						targets_fanin_new = dummy_targets_fanin_cone_new[d_cnt];
						targets_fanin_new->insert(targets_fanin_cone_new[t_cnt]->begin(), targets_fanin_cone_new[t_cnt]->end());
						node_it = targets_fanin_new->begin();
						while (node_it != targets_fanin_new->end()) {
							if (targets_fanout->find(*node_it) != targets_fanout->end())
								node_it = targets_fanin_new->erase(node_it);
							else
								++node_it;
						}

						prev_added = d_cnt;
					}
					else {
						std::cout << "merge target group: " << d_cnt << " -> " << prev_added << std::endl;

						targets_set = dummy_targets_nodes[prev_added];
						targets_set->insert(dummy_targets_nodes[d_cnt]->begin(), dummy_targets_nodes[d_cnt]->end());

						targets_po = dummy_targets_fanout_po[prev_added];
						targets_po->insert(dummy_targets_fanout_po[d_cnt]->begin(), dummy_targets_fanout_po[d_cnt]->end());

						targets_pi = dummy_targets_fanin_pi[prev_added];
						targets_pi->insert(dummy_targets_fanin_pi[d_cnt]->begin(), dummy_targets_fanin_pi[d_cnt]->end());

						targets_fanout = dummy_targets_fanout_cone[prev_added];
						targets_fanout->insert(dummy_targets_fanout_cone[d_cnt]->begin(), dummy_targets_fanout_cone[d_cnt]->end());

						targets_fanin_old = dummy_targets_fanin_cone_old[prev_added];
						targets_fanin_old->insert(dummy_targets_fanin_cone_old[d_cnt]->begin(), dummy_targets_fanin_cone_old[d_cnt]->end());
						nodecircuit::NodeSet::iterator node_it = targets_fanin_old->begin();
						while (node_it != targets_fanin_old->end()) {
							if (targets_fanout->find(*node_it) != targets_fanout->end())
								node_it = targets_fanin_old->erase(node_it);
							else
								++node_it;
						}

						targets_fanin_new = dummy_targets_fanin_cone_new[prev_added];
						targets_fanin_new->insert(dummy_targets_fanin_cone_new[d_cnt]->begin(), dummy_targets_fanin_cone_new[d_cnt]->end());
						node_it = targets_fanin_new->begin();
						while (node_it != targets_fanin_new->end()) {
							if (targets_fanout->find(*node_it) != targets_fanout->end())
								node_it = targets_fanin_new->erase(node_it);
							else
								++node_it;


							//delete d_cnt
							dummy_targets_nodes.erase(dummy_targets_nodes.begin() + d_cnt);
							dummy_targets_fanout_po.erase(dummy_targets_fanout_po.begin() + d_cnt);
							dummy_targets_fanin_pi.erase(dummy_targets_fanin_pi.begin() + d_cnt);
							dummy_targets_fanin_cone_old.erase(dummy_targets_fanin_cone_old.begin() + d_cnt);
							dummy_targets_fanin_cone_new.erase(dummy_targets_fanin_cone_new.begin() + d_cnt);
							dummy_targets_fanout_cone.erase(dummy_targets_fanout_cone.begin() + d_cnt);
							d_cnt--;
						}
					}
				}
			}
			if (empty) {
				std::cout << "new target group: " << dummy_targets_nodes.size() << " -> " << impl_circuit->targets[t_cnt]->name << std::endl;

				targets_set = new nodecircuit::NodeSet;
				targets_set->insert(impl_circuit->targets[t_cnt]);
				dummy_targets_nodes.push_back(targets_set);

				targets_po = new nodecircuit::NodeSet;
				//targets_po->insert(targets_fanout_po[t_cnt]->begin(), targets_fanout_po[t_cnt]->end());
				dummy_targets_fanout_po.push_back(targets_po);

				targets_pi = new nodecircuit::NodeSet;
				//targets_pi->insert(targets_fanin_pi[t_cnt]->begin(), targets_fanin_pi[t_cnt]->end());
				dummy_targets_fanin_pi.push_back(targets_pi);

				targets_fanin_old = new nodecircuit::NodeSet;
				//targets_fanin->insert(targets_fanin_cone[t_cnt]->begin(), targets_fanin_cone[t_cnt]->end());
				dummy_targets_fanin_cone_old.push_back(targets_fanin_old);

				targets_fanin_new = new nodecircuit::NodeSet;
				//targets_fanin->insert(targets_fanin_cone[t_cnt]->begin(), targets_fanin_cone[t_cnt]->end());
				dummy_targets_fanin_cone_new.push_back(targets_fanin_new);

				targets_fanout = new nodecircuit::NodeSet;
				//targets_fanout->insert(targets_fanout_cone[t_cnt]->begin(), targets_fanout_cone[t_cnt]->end());
				dummy_targets_fanout_cone.push_back(targets_fanout);
			}
			else if (prev_added < 0) {
				std::cout << "new target group: " << dummy_targets_nodes.size() << " -> " << impl_circuit->targets[t_cnt]->name << std::endl;

				targets_set = new nodecircuit::NodeSet;
				targets_set->insert(impl_circuit->targets[t_cnt]);
				dummy_targets_nodes.push_back(targets_set);

				targets_po = new nodecircuit::NodeSet;
				targets_po->insert(targets_fanout_po[t_cnt]->begin(), targets_fanout_po[t_cnt]->end());
				dummy_targets_fanout_po.push_back(targets_po);

				targets_pi = new nodecircuit::NodeSet;
				targets_pi->insert(targets_fanin_pi[t_cnt]->begin(), targets_fanin_pi[t_cnt]->end());
				dummy_targets_fanin_pi.push_back(targets_pi);

				targets_fanin_old = new nodecircuit::NodeSet;
				targets_fanin_old->insert(targets_fanin_cone_new[t_cnt]->begin(), targets_fanin_cone_new[t_cnt]->end());
				dummy_targets_fanin_cone_old.push_back(targets_fanin_old);

				targets_fanout = new nodecircuit::NodeSet;
				targets_fanout->insert(targets_fanout_cone[t_cnt]->begin(), targets_fanout_cone[t_cnt]->end());
				dummy_targets_fanout_cone.push_back(targets_fanout);
			}
		}
		// remove the nodes in the fanout cone off all other target
		nodecircuit::NodeSet* all_targets_fanout = new nodecircuit::NodeSet;
		for (int t_cnt_fo = 1; t_cnt_fo < impl_circuit->targets.size(); t_cnt_fo++) {
			all_targets_fanout->insert(targets_fanout_cone[t_cnt_fo]->begin(), targets_fanout_cone[t_cnt_fo]->end());
		}
		std::cout << "all targets fanout size : " << all_targets_fanout->size() << std::endl;
		for (int d_fi_cnt = 0; d_fi_cnt < dummy_targets_fanin_cone_old.size(); d_fi_cnt++) {
			targets_fanin_old = dummy_targets_fanin_cone_old[d_fi_cnt];
			std::cout << "old fanin size of target group " << d_fi_cnt << " : " << targets_fanin_old->size();
			nodecircuit::NodeSet::iterator node_it = targets_fanin_old->begin();
			while (node_it != targets_fanin_old->end()) {
				if (all_targets_fanout->find(*node_it) != all_targets_fanout->end())
					node_it = targets_fanin_old->erase(node_it);
				else
					++node_it;
			}
			std::cout << " -> " << targets_fanin_old->size() << std::endl;
		}

		for (int d_fi_cnt = 0; d_fi_cnt < dummy_targets_fanin_cone_new.size(); d_fi_cnt++) {
			targets_fanin_new = dummy_targets_fanin_cone_new[d_fi_cnt];
			std::cout << "old fanin size of target group " << d_fi_cnt << " : " << targets_fanin_new->size();
			nodecircuit::NodeSet::iterator node_it = targets_fanin_new->begin();
			while (node_it != targets_fanin_new->end()) {
				if (all_targets_fanout->find(*node_it) != all_targets_fanout->end())
					node_it = targets_fanin_new->erase(node_it);
				else
					++node_it;
			}
			std::cout << " -> " << targets_fanin_new->size() << std::endl;
		}

	}

	int NodeRestructurer::Process(std::vector<nodecircuit::NodeSet*> &dummy_targets_nodes, std::vector<nodecircuit::NodeSet*> &dummy_targets_fanin_cone,
		std::vector<nodecircuit::NodeSet*> &dummy_targets_fanout_cone,
		std::vector<nodecircuit::NodeSet*> &dummy_targets_fanout_po) {

		int size_inputs = 0;

		for (int dt_cnt = 0; dt_cnt < dummy_targets_nodes.size(); dt_cnt++) {
			std::cout << "processing targets:";
			nodecircuit::NodeSet::iterator temp_node_it = dummy_targets_nodes[dt_cnt]->begin();
			while (temp_node_it != dummy_targets_nodes[dt_cnt]->end()) {
				std::cout << " " << (*temp_node_it)->name;
				++temp_node_it;
			}
			std::cout << std::endl;
			/*if (dummy_targets_fanin_cone[dt_cnt]->size() == 0 || dummy_targets_fanin_pi[dt_cnt]->size() == 0) {
				nodecircuit::NodeSet::iterator tnode_it = dummy_targets_nodes[dt_cnt]->begin();
				while (tnode_it != dummy_targets_nodes[dt_cnt]->end()) {
					std::ofstream solution_file((*tnode_it)->name + "_solution");
					solution_file << (*tnode_it)->inputs[0]->name << std::endl;
					solution_file.close();
					++tnode_it;
				}
			}*/
			nodecircuit::NodeVector targets;
			nodecircuit::NodeSet::iterator tnode_it = dummy_targets_nodes[dt_cnt]->begin();
			while (tnode_it != dummy_targets_nodes[dt_cnt]->end()) {
				targets.push_back(*tnode_it);
				++tnode_it;
			}
			spec_circuit->outputs.clear();
			impl_circuit->outputs.clear();
			for (nodecircuit::NodeSet::iterator node_it = dummy_targets_fanout_po[dt_cnt]->begin();
				node_it != dummy_targets_fanout_po[dt_cnt]->end(); ++node_it) {
				impl_circuit->outputs.push_back(*node_it);
				spec_circuit->outputs.push_back(spec_circuit->all_nodes_map[(*node_it)->name]);
			}

			nodecircuit::NodeVector candids;
			unsigned long num_candids;

			CreateInitialCandidates(dummy_targets_fanin_cone[dt_cnt], candids, num_candids);

			//time_limit = 10; //need to be changed, recommended by Raku-san?
			//cout << "Time limit is set as: " << time_limit << endl;
			nodecircuit::NodeVector solution_nodes;
			std::vector< std::map < nodecircuit::Node*, bool > > in2cases;

			while (!targets.empty()) {
				int iteration_limit = 300;
				bool solution_found = false;

				solution_found = FindSolution(spec_circuit, impl_circuit, &candids, &targets, solution_nodes, in2cases, iteration_limit);

				if (solution_found) {
					size_inputs += pow(2, solution_nodes.size());
					impl_circuit->Restruct(targets[0], in2cases);
					auto it_del = targets.begin();
					targets.erase(it_del);
				}
				else {
					//impl_circuit.restore(dummy_targets_fanout_po[dt_cnt]);
					size_inputs += 9999;
					break;
				}
			}

		}
		return size_inputs;
	}

	void NodeRestructurer::CreateInitialCandidates(nodecircuit::NodeSet *target_fanin_nodes, nodecircuit::NodeVector &candidates, unsigned long &num_candids) {
		for (auto node : *target_fanin_nodes) {
			if (!node->is_input && !node->is_output) {
				candidates.push_back(node);
			}
		}
		num_candids = candidates.size();
		std::cout << "There are " << num_candids << " candidates" << std::endl;
	}

	//--------------find the new fanin and the new LUT--------------- 
	bool NodeRestructurer::FindSolution(nodecircuit::Circuit *spec_circuit, nodecircuit::Circuit* impl_circuit, nodecircuit::NodeVector* candidates, nodecircuit::NodeVector* targets,
		nodecircuit::NodeVector &solution_nodes, std::vector< std::map < nodecircuit::Node*, bool > > &in2cases, int iteration_limit) {
		bool solution_found = false;
		solution_nodes.clear();

		fanin_selection::FaninSelector find_fanin(impl_circuit, spec_circuit, candidates, targets, iteration_limit);
		solution_nodes = find_fanin.solution;
		//solution_nodes = { impl_circuit->GetNode("n3844"), impl_circuit->GetNode("n3840"), impl_circuit->GetNode("n7422"), impl_circuit->GetNode("n7060"),impl_circuit->GetNode("n10168") };//for debuggings
		solution_found = (solution_nodes.size() > 0); // why!

		if (solution_found) {
			std::cout << "solution found:" << std::endl;
			// sort solution nodes
			for (auto node : solution_nodes) {
				std::cout << node->name << ",";
			}
			std::cout << std::endl;

			nodecircuit::NodeVector empty_vector;

			CNFgen::CNFGenerator impl_cnf("impl", impl_circuit, targets, &solution_nodes);
			CNFgen::CNFGenerator spec_cnf("spec", spec_circuit, &empty_vector, &empty_vector);
			/*impl_circuit->WriteVerilog("test.v");
			DC::getDC dc(&solution_nodes, targets->at(0), &impl_cnf, &spec_cnf, impl_circuit);
			for (auto cs : find_fanin.in1cases)
				dc.removeCase(&cs, DC::getDC::IN2);
			for (auto cs : find_fanin.in0cases)
				dc.removeCase(&cs, DC::getDC::IN1);
			dc.solve();
			dc.solve_additional();
			dc.dumpCases();
			if (dc.in2cases.size() != dc.in2dccases.size())
				std::cout << "### in2dccases is used!!" << std::endl;
			//in2cases = dc.in2cases;
			in2cases = dc.in2dccases;*/

			DC::getDC dc(&solution_nodes, targets, &impl_cnf, &spec_cnf, impl_circuit);
			//for (auto cs : find_fanin.in1cases)
				//dc.removeCase(&cs, DC::getDC::IN2);
			dc.solve_multi();
			dc.dumpCases();
			in2cases = dc.in2cases;
		}
		else {
			std::cout << "solution not found!" << std::endl;
		}
		return solution_found;
	}

	//bool NodeIterator::FindSolutionSi(nodecircuit::Circuit *spec_circuit, nodecircuit::Circuit* impl_circuit, nodecircuit::NodeVector* candidates, nodecircuit::NodeVector* targets,
	//	nodecircuit::NodeVector &solution_nodes, std::vector< std::map < nodecircuit::Node*, bool > > &in2cases, int iteration_limit) {
	//	bool solution_found = false;
	//	solution_nodes.clear();

	//	// ADD THE CODE FOR FINDING SOLUTION FOR ONE TARGET HERE:
	//	fanin_selection::FaninSelectorSi find_fanin(impl_circuit, spec_circuit, candidates, targets, iteration_limit);
	//	solution_nodes = find_fanin.solution;
	//	solution_found = (solution_nodes.size() > 0); // why!

	//	if (solution_found) {
	//		std::cout << "solution found:" << std::endl;
	//		// sort solution nodes
	//		for (auto node : solution_nodes) {
	//			std::cout << node->name << ",";
	//		}
	//		std::cout << std::endl;

	//		nodecircuit::NodeVector empty_vector;

	//		CNFgen::CNFGenerator impl_cnf("impl", impl_circuit, targets, &solution_nodes);
	//		CNFgen::CNFGenerator spec_cnf("spec", spec_circuit, &empty_vector, &empty_vector);
	//		DC::getDC dc(&solution_nodes, targets, &impl_cnf, &spec_cnf, impl_circuit);
	//		//for (auto cs : find_fanin.in1cases)
	//			//dc.removeCase(&cs, DC::getDC::IN2);
	//		dc.solve_multi();
	//		dc.dumpCases();
	//		in2cases = dc.in2cases;
	//	}
	//	else {
	//		std::cout << "solution not found!" << std::endl;
	//	}
	//	return solution_found;
	//}

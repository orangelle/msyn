#include "setcover_m.h"

setcover_m::setcover_m(ClusterVector &clus, NodeVector&sig_inputs) {
	row_size = 0;
	col_size = 0;

	int col_cnt = 0;
	cands_c.reserve(clus.size());
	cands_c = clus;
	signal_inputs.reserve(sig_inputs.size());
	signal_inputs = sig_inputs;
	col_size = sig_inputs.size();
	for (int i = 0; i < col_size; i++) {
		node_pos[signal_inputs.at(i)->name] = i;
	}
}
setcover_m::setcover_m(NodeVector &sigs, NodeVector &sig_inputs )
{
	row_size = 0;
	col_size = 0;

	int col_cnt = 0;
	cands_n.reserve(sigs.size());
	cands_n = sigs;
	signal_inputs.reserve(sig_inputs.size());
	signal_inputs = sig_inputs;
	col_size = sig_inputs.size();
	for (int i = 0; i < col_size;i++) {
		node_pos[signal_inputs.at(i)->name] = i;
	}
}

int setcover_m::AddRows(vector<vector<bool>> &Rows)
{
	//cout << "adding rows" << endl;
	int i, j, prow_size;

	prow_size = row_size;
	row_size = Rows.size() + row_size;
	pvalues_row.resize(row_size);
	for (i = prow_size; i < row_size; i++) {
		pvalues_row[i].set_size(col_size);
	}

	if (row_size == Rows.size()) {
		pvalues_col.resize(col_size);
		for (j = 0; j < col_size; j++) {
			pvalues_col[j].set_size(row_size);
		}
	}
	else {
		for (j = 0; j < col_size; j++)
			pvalues_col[j].resize(row_size);
	}
	for (i = prow_size; i < row_size; i++) {
		for (j = 0; j < col_size; j++) {
			if (Rows[i - prow_size][j]) {
				pvalues_col[j].set(i);
				pvalues_row[i].set(j);
			}
		}
	}

	mask.set_size(row_size);
	return 1;
}

void setcover_m::GenOr(bitvector& mask_tmp,NodeVector inputs) {
	for (Node* node : inputs) {
		//cout << node->name<<": "<<node_pos[node->name]<<", ";
		mask_tmp.or_count(pvalues_col[node_pos[node->name]]);
	}
	//cout << endl;
}

void setcover_m::GenOr(bitvector& mask_tmp, NodeSet inputs) {
	for (Node* node : inputs) {
		//cout << node->name<<": "<<node_pos[node->name]<<", ";
		mask_tmp.or_count(pvalues_col[node_pos[node->name]]);
	}
	//cout << endl;
}

int setcover_m::SelectOnce(NodeVector & selected_nodes, NodeSet &selected_nodes_inputs) {
	int max_cover = 0;
	Node* node_selected;
	bitvector mask_selected;
	for (Node* node: cands_n) {
		bitvector mask_tmp(row_size);
		GenOr(mask_tmp, node->inputs);
		if (mask_tmp.mask_count(mask) > max_cover) {
			max_cover = mask_tmp.count();
			mask_selected.copy(mask_tmp);
			node_selected = node;
		}
	}
	cout << "max_cover: " << max_cover << endl;
	if (max_cover > 0) {
		mask.or_count(mask_selected);
		selected_nodes.push_back(node_selected);
		selected_nodes_inputs.insert(node_selected->inputs.begin(), node_selected->inputs.end());
		cout << "singal: " << node_selected->name << " (";
		for (Node* n : node_selected->inputs)
			cout << n->name << " ";
		cout << ") is selected" << endl;
		return 1;
	}
	else {
		return 0;
	}
}

int setcover_m::Select(NodeVector & selected_nodes, NodeSet &selected_nodes_inputs) {
	while (mask.count() != row_size) {
		if (!SelectOnce(selected_nodes, selected_nodes_inputs)) {
			cout << "no solution" << endl;
			selected_nodes.clear();
			selected_nodes_inputs.clear();
			return 0;
		}
	}
	cout << '\n';
	cout << "selected nodes: ";
	for (Node* node : selected_nodes) {
		cout << node->name << "(";
		for (Node* in : node->inputs) {
			cout << in->name << ", ";
		}
		cout << "), ";
	}
	cout << endl;
	cout << "selected nodes' inputs: ";
	for (Node* node : selected_nodes_inputs) {
		cout << node->name << ", ";
	}
	cout<<'\n';
	cout << endl;
}

int setcover_m::SelectOnce(ClusterVector & selected_clsts, NodeSet &selected_nodes_inputs) {
	int max_cover = 0;
	Cluster* clst_selected;
	bitvector mask_selected;
	for (Cluster* clst : cands_c) {
		bitvector mask_tmp(row_size);
		GenOr(mask_tmp, clst->input_nodes);
		if (mask_tmp.mask_count(mask) > max_cover) {
			max_cover = mask_tmp.count();
			mask_selected.copy(mask_tmp);
			clst_selected = clst;
		}
	}
	cout << "max_cover: " << max_cover << endl;
	if (max_cover > 0) {
		mask.or_count(mask_selected);
		selected_clsts.push_back(clst_selected);
		selected_nodes_inputs.insert(clst_selected->input_nodes.begin(), clst_selected->input_nodes.end());
		cout << "cluster: " << clst_selected->name << " (";
		for (Node* n : clst_selected->input_nodes)
			cout << n->name << " ";
		cout << ") is selected" << endl;
		return 1;
	}
	else {
		return 0;
	}
}

int setcover_m::Select(ClusterVector & selected_clsts, NodeSet &selected_nodes_inputs, int limit) {
	while (mask.count() != row_size) {
		if (!SelectOnce(selected_clsts, selected_nodes_inputs) || selected_clsts.size()>limit) {
			cout << "no solution" << endl;
			selected_clsts.clear();
			selected_nodes_inputs.clear();
			return 0;
		}
	}
	cout << '\n';
	cout << "selected nodes: ";
	for (Cluster* clst : selected_clsts) {
		cout << clst->name << "(";
		for (Node* in : clst->input_nodes) {
			cout << in->name << ", ";
		}
		cout << "), ";
	}
	cout << endl;
	cout << "selected nodes' inputs: ";
	for (Node* node : selected_nodes_inputs) {
		cout << node->name << ", ";
	}
	cout << '\n';
	cout << endl;
}



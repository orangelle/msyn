#ifndef MSYN_SETCOVER_M
#define MSYN_SETCOVER_M

#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include "node.h"
#include "bitop.h"
using namespace nodecircuit;
using namespace std;
using namespace bitprocess;

class setcover_m {
public:
	setcover_m(NodeVector &cands, NodeVector &cand_inputs);
	setcover_m(ClusterVector &cands, NodeVector &cand_inputs);
	~setcover_m() {};
	int AddRows(vector<vector<bool>> &Rows);
	void GenOr(bitvector& mask, NodeVector inputs);
	void GenOr(bitvector& mask, NodeSet inputs);
	//select nodes by greedy algorithm until all the rows are covered
	int Select(NodeVector & selected_nodes, NodeSet &selected_nodes_inputs);
	//select the node covering the most remaining rows
	int SelectOnce(NodeVector & selected_nodes, NodeSet &selected_nodes_inputs);
	//Select() for cluster
	int Select(ClusterVector & selected_nodes, NodeSet &selected_nodes_inputs, int limit);
	//SelectOnce() for cluster
	int SelectOnce(ClusterVector & selected_nodes, NodeSet &selected_nodes_inputs);

private:
	static const unsigned int CUR_MAX_EXACT_LIMIT = 4;
	static const unsigned int MINSIZE = 16;
	static const unsigned int MAXMINSIZE = 64;
	static const unsigned int WSIZE = 64;
	int row_size, col_size;
	NodeVector cands_n;
	ClusterVector cands_c;
	NodeVector signal_inputs;
	vector <bitvector> pvalues_row;
	vector <bitvector> pvalues_col;
	unordered_map <string, int> node_pos;
	bitvector mask;
};

#endif

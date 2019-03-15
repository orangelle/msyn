#ifndef SETCOVER_H_INCLUDED
#define SETCOVER_H_INCLUDED

#include <vector>
#include <map>
#include <set>
#include "bitop.h"
#include "node.h"

using namespace bitprocess;

namespace SC
{
class setcover{

public:
	//Read signal information. Only need to be called once!
	setcover(nodecircuit::NodeVector &signals_in);
	//Add new rows
	int AddRows(std::vector<std::vector<bool>> &Rows);
	//setcovering without considering weight
	int SolveMinCol(nodecircuit::NodeVector &selected_cands, nodecircuit::NodeVector &excluded_sigs, nodecircuit::NodeVector &included_sigs);
	//Try to reduce weight with same signal number
	int OptimizeWeight(nodecircuit::NodeVector &current_cands, nodecircuit::NodeVector &new_cands, nodecircuit::NodeVector &excluded_sigs, nodecircuit::NodeVector &included_sigs, const int t_limit);
	//exclude columns range from ex_col_start to ex_col_end
	int SolveMinCol_ExRange(nodecircuit::NodeVector &selected_cands, int ex_col_start, int ex_col_end);
	int OptimizeWeight_ExRange(nodecircuit::NodeVector &current_cands, nodecircuit::NodeVector &new_cands, int ex_col_start, int ex_col_end, const int t_limit);
  //setcovering without considering weight and without sorting and without include exclude!
  int SolveMinCol(nodecircuit::NodeVector &selected_cands);

	//Clear all data when want to rerun the entire debug process
	int ClearData()
	{
		row_size = 0;
		col_size = 0;
		pvalues_row.clear();
		pvalues_col.clear();
		signals.clear();
	}

	~setcover() {}

private:

	static const unsigned int CUR_MAX_EXACT_LIMIT = 4;
	static const unsigned int MINSIZE = 16;
	static const unsigned int MAXMINSIZE = 64;
	static const unsigned int WSIZE = 64;
	enum In_Ex_Type {
		No_In_Ex,
		IncludedFlag,
		ExcludedFlag
	};
	nodecircuit::NodeVector signals;
	int row_size, col_size;
    std::vector<bitvector>  pvalues_row;
    std::vector<bitvector> pvalues_col;


	void RemoveRowCol1(std::vector< bitvector > &pvalues_col, std::vector< bitvector > &pvalues_row, bitvector &rowmask, int colpos);

	bool IsSolutionLite(std::vector< bitvector > &pvalues_col, bitvector& rowmask, std::vector<int> &pos_list);

	bool IsSolutionLite2(std::vector< bitvector > &pvalues_col, bitvector& rowmask, int pos1, int pos2);

	bool IsSolutionLite3(std::vector< bitvector > &pvalues_col, bitvector& rowmask, int pos1, int pos2, int pos3);

	bool IsExSolution(std::vector<int> &sol, std::set< std::set<int> > &ex_sols);

	void AdjustExSolutions(std::set< std::set<int> > &ex_sols, int rpos);

	int GetBest2Solution(std::vector< bitvector > &pvalues_col, bitvector& rowmask, std::vector<int> &csum_sortindex, std::vector<int> &sol, std::set< std::set<int> > &ex_sols);

	int GetBest2Solution(std::vector< bitvector > &pvalues_col, bitvector& rowmask, std::set<int> &pos_list, std::vector<int> &sol, std::set< std::set<int> > &ex_sols);

	int GetBest2Selection(std::vector< bitvector > &pvalues_col, bitvector& rowmask, std::set<int> &pos_list, std::vector<int> &sol, std::set< std::set<int> > &ex_sols);

	int GetBest3Solution(std::vector< bitvector > &pvalues_col, bitvector& rowmask, std::vector<int> &csum_sortindex, std::vector<int> &sol, std::set< std::set<int> > &ex_sols);

	int GetBest3Solution(std::vector< bitvector > &pvalues_col, bitvector& rowmask, std::set<int> &pos_list, std::vector<int> &csum_sortindex, std::vector<int> &sol, std::set< std::set<int> > &ex_sols);

	int GetBest3Solution(std::vector< bitvector > &pvalues_col, bitvector& rowmask, std::set<int> &pos_list, std::vector<int> &sol, std::set< std::set<int> > &ex_sols);

	int GetBest4Solution(std::vector< bitvector > &pvalues_col, bitvector& rowmask, std::set<int> &pos_list, std::vector<int> &csum_sortindex, std::vector<int> &sol, std::set< std::set<int> > &ex_sols);

	int ReduceColCompress(std::vector< bitvector > &pvalues_col, std::vector<int> &sol, std::vector <In_Ex_Type> &inexcluded);

	int ReduceColReplace21(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol, std::vector <In_Ex_Type> &inexcluded);

	int ReduceColReplace21_Traverse(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol, std::vector <In_Ex_Type> &inexcluded);

	int ReduceColReplace31(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol, std::vector <In_Ex_Type> &inexcluded);

	int InExcludeSigs(std::vector< bitvector > &pvalues_row_copy, std::vector< bitvector > &pvalues_col_copy, std::vector<int> &cur_sol, nodecircuit::NodeVector &excluded_sigs, nodecircuit::NodeVector &included_sigs, std::vector <In_Ex_Type> &inexcluded, bitvector &rowmask, int wri_pvalue);

	int ReduceWeightReplace22(std::vector< bitvector> &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol, std::vector <In_Ex_Type> &inexcluded, const int t_limit);

	int printResult(std::vector< int > &solution);

	int ReduceWeightReplace11(std::vector <In_Ex_Type> &inexcluded, std::vector< bitvector > &pvalues_col, std::vector<int> &cur_sol);

	int ReduceWeightReplace11_Traverse(std::vector <In_Ex_Type> &inexcluded, std::vector< bitvector > &pvalues_col, std::vector<int> &cur_sol);

	int ReduceColCompress_All(std::vector< bitvector > &pvalues_col, std::vector<int> &sol, std::vector <In_Ex_Type> &inexcluded);

	int SortCol(std::vector< bitvector > &pvalues_col_copy, std::vector<int> &original_csum_sortindex);

	int SortWei(std::vector <int> &sol);

	int InExcludeSigs_Range(std::vector< bitvector > &pvalues_row_copy, std::vector< bitvector > &pvalues_col_copy, std::vector <int> &cur_sol, int ex_col_start, int ex_col_end, std::vector <In_Ex_Type> &inexcluded, int wri_pvalue);

  int ReduceColCompress_All(std::vector< bitvector > &pvalues_col, std::vector<int> &sol);

  int ReduceColCompress(std::vector< bitvector > &pvalues_col, std::vector<int> &sol);

  int ReduceColReplace21(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol);

  int ReduceColReplace21_Traverse(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol);

  int ReduceColReplace31(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol);

	int PartitionCol(std::vector <int> &original_csum_sortindex, int low, int high);

	int PartitionWei(std::vector <int> &sol, int low, int high);
};
}

#endif

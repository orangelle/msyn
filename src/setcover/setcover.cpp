#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <algorithm>
#include <time.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <map>
#include <string.h>
#include <math.h>
#include "bitop.h"
#include "setcover.h"
#include "node.h"

using namespace std;
using namespace bitprocess;

namespace SC{

void setcover::RemoveRowCol1(vector< bitvector > &pvalues_col, vector< bitvector > &pvalues_row, bitvector& rowmask, int colpos) {
	for (int i = 0; i < pvalues_col.size(); i++)
		pvalues_col[i].mask_count(rowmask);
	//for (int j = 0; j < pvalues_row.size(); j++)
	//  pvalues_row[j].reset(colpos);
	int nn = rowmask.next_set(-1);
	while (nn < rowmask.size()) {
		pvalues_row[nn].clear();
		nn = rowmask.next_set(nn);
	}

}

//IsSolutionLite(pvalues_col, nomask, prev_sol)
bool setcover::IsSolutionLite(vector< bitvector > &pvalues_col, bitvector& rowmask, vector<int> &pos_list) {
	int i, pos;
	for (i = 0; i < rowmask.word_size() - 1; i++) {
		uint64_t res = rowmask.get_word(i);
		for (pos = 0; res != UINT64_MAX && pos < pos_list.size(); pos++)
			res |= pvalues_col[pos_list[pos]].get_word(i);
		if (res != UINT64_MAX)
			return false;
	}
	uint64_t res = rowmask.get_word(i);
	for (pos = 0; pos < pos_list.size(); pos++)
		res |= pvalues_col[pos_list[pos]].get_word(i);
	int res_cnt = count64(res);
	return (res_cnt%WSIZE == rowmask.size() % WSIZE);
}

bool setcover::IsSolutionLite2(vector<bitvector> &pvalues_col, bitvector& rowmask, int pos1, int pos2) {
	int i;
	for (i = 0; i < rowmask.word_size() - 1; i++) {
		uint64_t res = rowmask.get_word(i) | pvalues_col[pos1].get_word(i) | pvalues_col[pos2].get_word(i);
		if (res != UINT64_MAX)
			return false;
	}
	uint64_t res = rowmask.get_word(i) | pvalues_col[pos1].get_word(i) | pvalues_col[pos2].get_word(i);
	int res_cnt = count64(res);
	return (res_cnt%WSIZE == rowmask.size() % WSIZE);

}

bool setcover::IsSolutionLite3(vector< bitvector > &pvalues_col, bitvector& rowmask, int pos1, int pos2, int pos3) {
	int i;
	for (i = 0; i < rowmask.word_size() - 1; i++) {
		uint64_t res = rowmask.get_word(i) | pvalues_col[pos1].get_word(i) | pvalues_col[pos2].get_word(i) | pvalues_col[pos3].get_word(i);
		if (res != UINT64_MAX)
			return false;
	}
	uint64_t res = rowmask.get_word(i) | pvalues_col[pos1].get_word(i) | pvalues_col[pos2].get_word(i) | pvalues_col[pos3].get_word(i);
	int res_cnt = count64(res);
	return (res_cnt%WSIZE == rowmask.size() % WSIZE);

}

bool setcover::IsExSolution(vector<int> &sol, set< set<int> > &ex_sols) {
	if (ex_sols.size() == 0)
		return false;
	if (sol.size() > CUR_MAX_EXACT_LIMIT)
		return false; // DO NOT CHECK! because of overhead?!
	set<int> cursol;
	for (int i = 0; i < sol.size(); i++)
		cursol.insert(sol[i]);
	bool ret = (ex_sols.find(cursol) != ex_sols.end());

	return ret;
}

void setcover::AdjustExSolutions(set< set<int> > &ex_sols, int rpos) {
	if (ex_sols.size() == 0)
		return;
/*	printf("removing: %i\n", rpos);
	for (set< set<int> >::iterator it = ex_sols.begin(); it != ex_sols.end(); ++it)
		printf("%i ", (*it).size());
	printf("\n"); */
	for (set< set<int> >::iterator it = ex_sols.begin(); it != ex_sols.end(); ++it) {
		set<int>::iterator it2 = (*it).find(rpos);
		if (it2 != (*it).end()) {
			set<int> s = *it;
			s.erase(rpos);
			ex_sols.insert(s);
		}
	}
/*	for (set< set<int> >::iterator it = ex_sols.begin(); it != ex_sols.end(); ++it)
		printf("%i ", (*it).size());
	printf("\n"); */
}

int setcover::GetBest2Solution(vector< bitvector > &pvalues_col, bitvector& rowmask, vector<int> &csum_sortindex, vector<int> &sol, set< set<int> > &ex_sols) {
	sol.clear();
	int cur_weight = 0;
	int col_size = csum_sortindex.size();
	int maxsize = rowmask.size() - rowmask.count();
	int halfsize = maxsize / 2;
	int midindex = 0;
	if (pvalues_col[csum_sortindex[col_size - 1]].count() >= halfsize)
		midindex = col_size - 1;
	else
		while (pvalues_col[csum_sortindex[midindex]].count() >= halfsize)
			midindex++;
	for (int ii = 0; ii < midindex; ii++) {
		int i = csum_sortindex[ii];
		int rem_weight = maxsize - pvalues_col[i].count();
		int jj = col_size - 1;
		for (; jj > ii; jj--) {
			int j = csum_sortindex[jj];
			if (pvalues_col[j].count() >= rem_weight)
				break;
		}
		for (; jj > ii; jj--) {
			int j = csum_sortindex[jj];
			if (IsSolutionLite2(pvalues_col, rowmask, i, j)) {
				sol.push_back(i);
				sol.push_back(j);
				if (!IsExSolution(sol, ex_sols))
					return cur_weight;
				else
					sol.clear();
			}
		}
	}

	return cur_weight;
}

int setcover::GetBest2Solution(vector< bitvector > &pvalues_col, bitvector& rowmask, set<int> &pos_list, vector<int> &sol, set< set<int> > &ex_sols) {
	sol.clear();
	int cur_weight = 0;
	int maxsize = rowmask.size() - rowmask.count();
	for (set<int>::iterator it = pos_list.begin(); it != pos_list.end(); ++it) {
		int i = *it;
		//if (pvalues_col[i].count() == 0)
		//  continue;
		for (int j = 0; j < col_size; j++) {
			//if (pvalues_col[j].count() == 0)
			//  continue;
			cur_weight = pvalues_col[i].count() + pvalues_col[j].count();
			if (/*i != j &&*/ cur_weight >= maxsize) {
				if (IsSolutionLite2(pvalues_col, rowmask, i, j)) {
					sol.push_back(i);
					sol.push_back(j);
					if (!IsExSolution(sol, ex_sols))
						return cur_weight;
					else
						sol.clear();
				}
			}
		}
	}

	return cur_weight;
}

int setcover::GetBest2Selection(vector< bitvector > &pvalues_col, bitvector& rowmask, set<int> &pos_list, vector<int> &sol, set< set<int> > &ex_sols) {
	sol.clear();
	int ovp_weight = 0;
	int max_weight = 0;
	//int maxsize = rowmask.size() - rowmask.count();
	//int col_size = pvalues_col.size();
	for (set<int>::iterator it1 = pos_list.begin(); it1 != pos_list.end(); ++it1) {
		int i = *it1;
		set<int>::iterator it2 = it1;
		for (++it2; it2 != pos_list.end(); ++it2) {
			int j = *it2;
			int cur_weight = count_or(pvalues_col[i], pvalues_col[j], rowmask);
			int cur_ovp = count_and(pvalues_col[i], pvalues_col[j], rowmask);
			if (cur_weight > max_weight) {
				//printf("%i,%i . cur,ovp = %i,%i ---- %i,%i\n",i,j,cur_weight, cur_ovp,max_weight,ovp_weight);
				sol.clear();
				sol.push_back(i);
				sol.push_back(j);
				if (IsExSolution(sol, ex_sols)) {
					max_weight = 0;
					ovp_weight = 0;
					sol.clear();
				}
				else {
					max_weight = cur_weight;
					ovp_weight = cur_ovp;
				}
			}
			else if (cur_weight == max_weight && cur_ovp < ovp_weight) {
				//printf("%i,%i . cur,ovp = %i,%i ---- %i,%i\n",i,j,cur_weight, cur_ovp,max_weight,ovp_weight);
				sol.clear();
				sol.push_back(i);
				sol.push_back(j);
				if (IsExSolution(sol, ex_sols)) {
					max_weight = 0;
					ovp_weight = 0;
					sol.clear();
				}
				else {
					max_weight = cur_weight;
					ovp_weight = cur_ovp;
				}
			}
		}
	}

//	printf("best %i,%i\n", max_weight, ovp_weight);
	return max_weight;
}

int setcover::GetBest3Solution(vector< bitvector > &pvalues_col, bitvector& rowmask, vector<int> &csum_sortindex, vector<int> &sol, set< set<int> > &ex_sols) {
	sol.clear();
	int cur_weight = 0;
	int maxsize = rowmask.size() - rowmask.count();
	int col_size = csum_sortindex.size();
	int midsize = maxsize / 3;
	int midindex = 0;
	if (pvalues_col[csum_sortindex[col_size - 2]].count() >= midsize)
		midindex = col_size - 2;
	else
		while (pvalues_col[csum_sortindex[midindex]].count() >= midsize)
			midindex++;
	for (int ii = 0; ii < midindex; ii++) {
		for (int kk = col_size - 1; /*kk > jj+1 &&*/ kk > midindex; kk--) {
			for (int jj = ii + 1; jj < kk /*midindex*2*/; jj++) {
				int i = csum_sortindex[ii];
				int j = csum_sortindex[jj];
				int k = csum_sortindex[kk];
				cur_weight = pvalues_col[i].count() + pvalues_col[j].count() + pvalues_col[k].count();
				if (cur_weight < maxsize)
					jj = kk;
				else if (cur_weight >= maxsize) {
					if (IsSolutionLite3(pvalues_col, rowmask, i, j, k)) {
						sol.push_back(i);
						sol.push_back(j);
						sol.push_back(k);
						if (!IsExSolution(sol, ex_sols))
							return cur_weight;
						else
							sol.clear();
					}
				}
			}
		}
	}

	return cur_weight;
}

int setcover::GetBest3Solution(vector< bitvector > &pvalues_col, bitvector& rowmask, set<int> &pos_list, vector<int> &csum_sortindex, vector<int> &sol, set< set<int> > &ex_sols) {
	sol.clear();
	int cur_weight = 0;
	int col_size = csum_sortindex.size();
	int maxsize = rowmask.size() - rowmask.count();
	int midsize = maxsize / 3;
	int midindex = 0;
	if (pvalues_col[csum_sortindex[col_size - 2]].count() >= midsize)
		midindex = col_size - 2;
	else
		while (pvalues_col[csum_sortindex[midindex]].count() >= midsize)
			midindex++;
	for (set<int>::iterator it = pos_list.begin(); it != pos_list.end(); ++it) {
		int k = *it;
		for (int ii = 0; ii < midindex; ii++) {
			int i = csum_sortindex[ii];
			if (i == k)
				continue;
			for (int jj = col_size - 1; jj > ii; jj--) {
				int j = csum_sortindex[jj];
				if (j == k)
					continue;
				cur_weight = pvalues_col[i].count() + pvalues_col[j].count() + pvalues_col[k].count();
				if (cur_weight >= maxsize) {
					if (IsSolutionLite3(pvalues_col, rowmask, i, j, k)) {
						sol.push_back(i);
						sol.push_back(j);
						sol.push_back(k);
						if (!IsExSolution(sol, ex_sols))
							return cur_weight;
						else
							sol.clear();
					}
				}
			}
		}
	}

	return cur_weight;
}

int setcover::GetBest3Solution(vector< bitvector > &pvalues_col, bitvector& rowmask, set<int> &pos_list, vector<int> &sol, set< set<int> > &ex_sols) {
	sol.clear();
	int cur_weight = 0;
	int maxsize = rowmask.size() - rowmask.count();
	for (set<int>::iterator it = pos_list.begin(); it != pos_list.end(); ++it) {
		int i = *it;
		//if (pvalues_col[i].count() == 0)
		//  continue;
		for (int j = 0; j < col_size - 1; j++) {
			if (pvalues_col[j].count() == 0)
				continue;
			if (i == j)
				continue;
			for (int k = j + 1; k < col_size; k++) {
				if (pvalues_col[k].count() == 0)
					continue;
				if (i == k)
					continue;
				cur_weight = pvalues_col[i].count() + pvalues_col[j].count() + pvalues_col[k].count();
				if (cur_weight >= maxsize) {
					if (IsSolutionLite3(pvalues_col, rowmask, i, j, k)) {
						sol.push_back(i);
						sol.push_back(j);
						sol.push_back(k);
						if (!IsExSolution(sol, ex_sols))
							return cur_weight;
						else
							sol.clear();
					}
				}
			}
		}
	}

	return cur_weight;
}

int setcover::GetBest4Solution(vector< bitvector > &pvalues_col, bitvector& rowmask, set<int> &pos_list, vector<int> &csum_sortindex, vector<int> &sol, set< set<int> > &ex_sols) {
	vector<int> cur_sol;
	sol.clear();
	int cur_weight = 0;
	int col_size = csum_sortindex.size();
	int maxsize = rowmask.size() - rowmask.count();
	int midsize = maxsize / 3;
	int midindex = 0;
	if (pvalues_col[csum_sortindex[col_size - 1]].count() >= midsize)
		midindex = col_size - 1;
	else
		while (pvalues_col[csum_sortindex[midindex]].count() >= midsize)
			midindex++;
	for (set<int>::iterator it1 = pos_list.begin(); it1 != pos_list.end(); ++it1) {
		int k1 = *it1;
		set<int>::iterator it2 = it1;
		for (++it2; it2 != pos_list.end(); ++it2) {
			int k2 = *it2;
			for (int ii = 0; ii < midindex; ii++) {
				int i = csum_sortindex[ii];
				if (i == k1 || i == k2)
					continue;
				for (int jj = col_size - 1; jj > ii; jj--) {
					int j = csum_sortindex[jj];
					if (j == k1 || j == k2)
						continue;
					cur_weight = pvalues_col[i].count() + pvalues_col[j].count() + pvalues_col[k1].count() + pvalues_col[k2].count();
					if (cur_weight >= maxsize) {
						cur_sol.clear();
						cur_sol.push_back(i);
						cur_sol.push_back(j);
						cur_sol.push_back(k1);
						cur_sol.push_back(k2);
						if (IsSolutionLite(pvalues_col, rowmask, cur_sol)) {
							if (!IsExSolution(cur_sol, ex_sols)) {
								//sol.clear();
								sol.push_back(i);
								sol.push_back(j);
								sol.push_back(k1);
								sol.push_back(k2);
								return cur_weight;
							}
						}
					}
				}
			}
		}
	}

	return cur_weight;
}

int setcover::ReduceColCompress(vector< bitvector > &pvalues_col, vector<int> &sol, vector <In_Ex_Type> &inexcluded) {

	bitvector nomask(pvalues_col[0].size());
	for (vector<int>::iterator it = sol.begin(); it != sol.end(); ) {
		{
			vector<int> tempsol;
			for (int j = 0; j < sol.size(); j++) {
				if (sol[j] != *it)
					tempsol.push_back(sol[j]);
			}
			if (IsSolutionLite(pvalues_col, nomask, tempsol)) {
				it = sol.erase(it);
			/*	printf("new sol(Co): ");
				for (int m = 0; m < sol.size(); m++)
					printf("%i ", sol[m]);
				printf("\n"); */
			}
			else
				++it;
		}
	}

	return sol.size();
}

int setcover::ReduceColCompress_All(vector< bitvector > &pvalues_col, vector<int> &sol, vector <In_Ex_Type> &inexcluded) {
	vector < vector<int> > all_sol;
	int pre_sol_num = 0;
	all_sol.push_back(sol);
	bitvector nomask(pvalues_col[0].size());


	while (pre_sol_num < all_sol.size())
	{
		int i = 0;
		vector < vector<int> >::iterator iter = all_sol.begin();
		while (i < pre_sol_num)
		{
			iter = all_sol.erase(iter);
			i++;
		}
		pre_sol_num = all_sol.size();
		for (int i = 0;i < pre_sol_num;i++)
		{
			for (vector<int>::iterator it = all_sol[i].begin(); it != all_sol[i].end(); it++)
			{
				{
					vector<int> tempsol;
					for (int j = 0; j < all_sol[i].size(); j++)
						if (all_sol[i][j] != *it)
							tempsol.push_back(all_sol[i][j]);
					if (find(all_sol.begin(), all_sol.end(), tempsol) != all_sol.end())
						continue;
					if (IsSolutionLite(pvalues_col, nomask, tempsol))
						all_sol.push_back(tempsol);
				}

			}
		}
	}

	if (all_sol[0].size() < sol.size())
	{
		int best_sol_size = all_sol[0].size();
		int best_sol_index = 0;
		// cout << "new sol(Compress_All)" << endl;
		// int best_wei = printResult(all_sol[0]);
		 int best_wei = 0;
		 for (vector<int>::iterator iter = all_sol[0].begin();iter != all_sol[0].end();iter++)
				 best_wei = best_wei + signals[*iter]->weight;

		for (int i = 1;i < all_sol.size();i++)
		{

			 int cur_wei = 0;
			 for (vector<int>::iterator iter = all_sol[i].begin();iter != all_sol[i].end();iter++)
					 cur_wei = cur_wei + signals[*iter]->weight;
			if (all_sol[i].size() < best_sol_size || (all_sol[i].size() == best_sol_size && cur_wei < best_wei))
			{
				best_wei = cur_wei;
				best_sol_index = i;
				best_sol_size = all_sol[i].size();
			}

		}
		sol.clear();
		sol.assign(all_sol[best_sol_index].begin(), all_sol[best_sol_index].end());
		cout << "Final new sol(Compress_All)" << endl;
	//	printResult(sol);
	}


	return sol.size();
}


int setcover::ReduceColReplace21(vector< bitvector > &pvalues_col, vector<int> &csum_sortindex, vector<int> &sol, vector <In_Ex_Type> &inexcluded) {

	for (int i = 0; i < sol.size() - 1; i++) {
		for (int j = i + 1; j < sol.size(); j++) {
			{
				bitvector both;
				both.copy(pvalues_col[sol[i]]);
				int cnt = both.or_count(pvalues_col[sol[j]]);		//both = col_i | col_j
				for (int m = 0; m < sol.size(); m++) {
					if (m != i && m != j)
						cnt = both.mask_count(pvalues_col[sol[m]]); //!!!both = the rows only covered by col_i and col_j!!!
				}
				int k = 0;
				while (k < csum_sortindex.size() && pvalues_col[csum_sortindex[k]].count() >= cnt)
					k++;
				k--;
				while (k >= 0) {
					int newcol = csum_sortindex[k];
					if (find(sol.begin(), sol.end(), newcol) != sol.end()) {
						k--;
						continue;
					}
					if (cnt <= pvalues_col[newcol].count() && inexcluded[newcol] != ExcludedFlag) {
						int cnt2 = count_and(both, pvalues_col[newcol]);
						if (cnt2 == cnt) {
							sol[j] = sol[sol.size() - 1];
							sol.pop_back();
							sol[i] = sol[sol.size() - 1];
							sol.pop_back();
              sol.push_back(newcol);
              SortWei(sol);
							j = sol.size() + 2;
							i = -1;
						/*	printf("new sol(Re21): ");
							for (int m = 0; m < sol.size(); m++)
								printf("%i ", sol[m]);
							printf("\n");  */
							break;
						}
						else
							k--;
					}
				}
			}
		}
	}
	return sol.size();
}

int setcover::ReduceColReplace21_Traverse(vector< bitvector > &pvalues_col, vector<int> &csum_sortindex, vector<int> &sol, vector <In_Ex_Type> &inexcluded)
{
	int best_sol_size = sol.size();
	vector <int> best_sol;

	for (int start_sig = 0; start_sig < sol.size();start_sig++)
	{
		vector<int>tmp_sol(sol.size());

		for (int i = start_sig;i < sol.size();i++)
			tmp_sol[i - start_sig] = sol[i];
		for (int i = 0;i < start_sig;i++)
			tmp_sol[sol.size() - start_sig + i] = sol[i];

		ReduceColReplace21(pvalues_col, csum_sortindex, tmp_sol, inexcluded);

		if (best_sol_size > tmp_sol.size())
		{
			best_sol_size = tmp_sol.size();
			best_sol.clear();
			best_sol.assign(tmp_sol.begin(), tmp_sol.end());
		}
	}

	if (best_sol_size < sol.size())
	{
		sol.clear();
		sol.assign(best_sol.begin(), best_sol.end());
		cout << "------Traverse final new sol(Re21)-------" << endl;
	//	printResult(sol);
	}

}

int setcover::ReduceColReplace31(vector< bitvector > &pvalues_col, vector<int> &csum_sortindex, vector<int> &sol, vector <In_Ex_Type> &inexcluded) {
	for (int i = 0; i < sol.size() - 2; i++) {
		for (int j1 = i + 1; j1 < sol.size() - 1; j1++) {
			for (int j2 = j1 + 1; j2 < sol.size(); j2++) {
				{
					bitvector both;
					both.copy(pvalues_col[sol[i]]);
					both.or_count(pvalues_col[sol[j1]]);
					int cnt = both.or_count(pvalues_col[sol[j2]]);
					for (int m = 0; m < sol.size(); m++) {
						if (m != i && m != j1 && m != j2)
							cnt = both.mask_count(pvalues_col[sol[m]]);
					}
					int k = 0;
					while (k < csum_sortindex.size() && pvalues_col[csum_sortindex[k]].count() >= cnt)
						k++;
					k--;
					while (k >= 0 ) {
						int newcol = csum_sortindex[k];
						if (find(sol.begin(), sol.end(), newcol) != sol.end()) {	//make sure newcol isn't inside current sol
							k--;
							continue;
						}
						if (cnt <= pvalues_col[newcol].count() && inexcluded[newcol] != ExcludedFlag) { //don't replace with excluded signal
							int cnt2 = count_and(both, pvalues_col[newcol]);
							if (cnt2 == cnt) {
								sol[j2] = sol[sol.size() - 1];
								sol.pop_back();
								sol[j1] = sol[sol.size() - 1];
								sol.pop_back();
								sol[i] = sol[sol.size() - 1];
								sol.pop_back();
								sol.push_back(newcol);
                                SortWei(sol);
								j1 = sol.size() + 2;
								j2 = sol.size() + 2;
								i = -1;
							/*	printf("new sol(Re31): ");
								for (int m = 0; m < sol.size(); m++)
									printf("%i ", sol[m]);
								printf("\n");  */
								break;
							}
							else
								k--;
						}
					}
				}
			}
		}
	}

	//printf("\n");
	return sol.size();
}

int setcover::printResult(vector< int > &solution)
{
	int weight = 0;

	for (vector<int>::iterator iter = solution.begin();iter != solution.end();iter++)
	{
		cout << signals[*iter]->name << "(COL" << *iter << " W" << signals[*iter]->weight << ") ";
		weight = weight + signals[*iter]->weight;
	}
	cout << endl;
	cout << "The signal number is: " << solution.size() << endl;
	return weight;
}

setcover::setcover(nodecircuit::NodeVector &signals_in)
{
	row_size = 0;
	col_size = 0;

	int col_cnt = 0;
  signals.reserve(signals_in.size());
  signals = signals_in;
  col_size = signals.size();
}

int setcover::AddRows(vector<vector<bool>> &Rows)
{
	int i, j, prow_size;

	prow_size = row_size;
	row_size = Rows.size() + row_size;
    pvalues_row.resize(row_size);
	for (i = prow_size; i < row_size; i++) {
		pvalues_row[i].set_size(col_size);
	}

	if (row_size == Rows.size()){
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
			if (Rows[i-prow_size][j]) {
				pvalues_col[j].set(i);
				pvalues_row[i].set(j);
			}
		}
	}

	return 1;
}

int setcover::InExcludeSigs(vector< bitvector > &pvalues_row_copy, vector< bitvector > &pvalues_col_copy, vector<int> &cur_sol, nodecircuit::NodeVector &excluded_sigs, nodecircuit::NodeVector &included_sigs, vector <In_Ex_Type> &inexcluded, bitvector &rowmask, int wri_pvalue)
{
	vector <int> excludecol, includecol;


	if (excluded_sigs.size() != 0 && included_sigs.size() != 0)
	{
		nodecircuit::NodeVector intersection;
		set_intersection(excluded_sigs.begin(),excluded_sigs.end(),included_sigs.begin(),included_sigs.end(),inserter(intersection,intersection.begin()));
		if (intersection.size() > 0)
		{
			cout << "****************** Warning: Signals are included and excluded simultaneously!!!-----. ";
			for (int i = 0; i < intersection.size(); i++)
			{
				cout << intersection[i]->name << "(W" << intersection[i]->weight << ")";
				nodecircuit::NodeVector::iterator findsig1 = find(excluded_sigs.begin(), excluded_sigs.end(), intersection[i]);
				excluded_sigs.erase(findsig1++);
				nodecircuit::NodeVector::iterator findsig2 = find(included_sigs.begin(), included_sigs.end(), intersection[i]);
				included_sigs.erase(findsig2++);
			}
			cout << "******************" << endl;
		}
	}

	int excludedCnt = 0, includedCnt = 0;
	for (unsigned int col = 0; col < signals.size(); col++)
	{
		if (excludedCnt < excluded_sigs.size() && excluded_sigs.size() != 0 && signals[col]->weight)
		{
			nodecircuit::NodeVector::iterator iter = find(excluded_sigs.begin(), excluded_sigs.end(), signals[col]);
			if (iter != excluded_sigs.end())
			{
				excludecol.push_back(col);
				inexcluded[col] = ExcludedFlag;
				excludedCnt++;
			}
		}
		else if (!signals[col]->weight)
		{
			excludecol.push_back(col);
			inexcluded[col] = ExcludedFlag;
		}

		if (includedCnt < included_sigs.size() && included_sigs.size() != 0 && signals[col]->weight) //signals will not be included when weight is 0
		{
			nodecircuit::NodeVector::iterator iter = find(included_sigs.begin(), included_sigs.end(), signals[col]);
			if (iter != included_sigs.end())
			{
				includecol.push_back(col);
				inexcluded[col] = IncludedFlag;
				includedCnt++;
			}
		}

		if (includedCnt == included_sigs.size() && excludedCnt == excluded_sigs.size())
			break;
	}

	if (excludecol.size() != 0)
	{
		cout << "----Excluded signals: ";
		for (int i = 0; i < excludecol.size(); i++)
			cout << signals[excludecol[i]]->name << "(W" << signals[excludecol[i]]->weight << " Col" << excludecol[i] << ") ";
		cout << endl;

//		if (wri_pvalue) {
			for (int i = 0; i < excludecol.size(); i++)
			{
				pvalues_col_copy[excludecol[i]].clear();
				for (int j = 0;j < row_size;j++)
					pvalues_row_copy[j].reset(excludecol[i]);
			}
			for (int i = 0; i < row_size; i++) {
				if (pvalues_row_copy[i].count() == 0) {
					cout << "*********Warning: NO SOLUTION if these columns are excluded!*********" << endl;
					return 0;
				}
			}
		}
//	}

	if (includecol.size() != 0)
	{
		cout << "----Included signals: ";
		for (int i = 0; i < includecol.size(); i++)
			cout << signals[includecol[i]]->name << "(W" << signals[includecol[i]]->weight << " Col" << includecol[i] << ") ";

		for (int i = 0; i < includecol.size(); i++) {
			vector <int>::iterator iter = find(cur_sol.begin(),cur_sol.end(), includecol[i]);
			if (iter == cur_sol.end())
				cur_sol.push_back(includecol[i]);
		}
		if (wri_pvalue) {
			for (int i = 0; i < includecol.size(); i++)
				rowmask.or_count(pvalues_col_copy[includecol[i]]);
			RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, 0);
		}

	}

	for (vector <int>::iterator iter = cur_sol.begin();iter != cur_sol.end();iter++)
		if (inexcluded[*iter] == ExcludedFlag)
			cout << "***********Warning: the input solution " << signals[*iter]->name << "(Col " << *iter << ") " << "is insize the excluding range*********" << endl;


	return 1;
}

int setcover::PartitionCol(vector <int> &original_csum_sortindex, int low, int high){
    int pivot=original_csum_sortindex[low];
    while(low<high){
        while(low<high && pvalues_col[original_csum_sortindex[high]].count()<=pvalues_col[pivot].count())
            high--;
        original_csum_sortindex[low]=original_csum_sortindex[high];
        while(low<high && pvalues_col[original_csum_sortindex[low]].count()>=pvalues_col[pivot].count())
            low++;
        original_csum_sortindex[high]=original_csum_sortindex[low];
    }
    original_csum_sortindex[low]=pivot;
    return low;
}

int setcover::SortCol(vector< bitvector > &pvalues_col_copy, vector<int> &original_csum_sortindex)
{
	for(int col=0;col<pvalues_col_copy.size();col++)
		if(pvalues_col_copy[col].count())
				original_csum_sortindex.push_back(col);

	vector <int> pivots;
	int low,high,mid;
	low=0;
	high=original_csum_sortindex.size()-1;
	mid=PartitionCol(original_csum_sortindex,low,high);
	if(low<mid-1){
			pivots.push_back(low);
			pivots.push_back(mid-1);
	}
	if(high>mid+1){
			pivots.push_back(mid+1);
			pivots.push_back(high);
	}

	while(pivots.size()){
			high=pivots[pivots.size()-1];
			pivots.pop_back();
			low=pivots[pivots.size()-1];
			pivots.pop_back();
			mid=PartitionCol(original_csum_sortindex,low,high);
			if(low<mid-1){
					pivots.push_back(low);
					pivots.push_back(mid-1);
			}
			if(high>mid+1){
					pivots.push_back(mid+1);
					pivots.push_back(high);
			}
	}
	/*
	int curtest, lltest;
	for (int k = 0; k < col_size; k++) {
		int cur = pvalues_col_copy[k].count();
		curtest = cur;
		if (cur > 0) {
			original_csum_sortindex.push_back(k);
			int ll = original_csum_sortindex.size() - 2;
			lltest = ll;
			while (ll >= 0) {
				if (cur > pvalues_col_copy[original_csum_sortindex[ll]].count()) {
					original_csum_sortindex[ll + 1] = original_csum_sortindex[ll];
					ll--;
					lltest--;
					if (ll == -1) {
						original_csum_sortindex[0] = k;
					}
				}
				else {
					original_csum_sortindex[ll + 1] = k;
					ll = -1;
					lltest = -1;
				}
			}
		}
	}*/
	return 1;
}

int setcover::SolveMinCol(nodecircuit::NodeVector &selected_cands, nodecircuit::NodeVector &excluded_sigs, nodecircuit::NodeVector &included_sigs)
{
	//clock_t t1 = clock();

	int i, j, k;

	vector<int> cur_sol;

	cout << "----------------Setcovering Start(No weight)----------------" << endl;
    cout << "colsize=" << col_size << "  rowsize=" << row_size << endl;
	vector<int> prev_sol;
	vector<int> ex_pos;
	set< set<int> > ex_sols;
	set<int> tmp_sol;
	j = 2;
	int p = 0;

	vector< bitvector > pvalues_row_copy, pvalues_col_copy;

	pvalues_row_copy.resize(row_size);
	for (i = 0; i < row_size; i++) {
		pvalues_row_copy[i].copy(pvalues_row[i]);
	}
	pvalues_col_copy.resize(col_size);
	for (j = 0; j < col_size; j++) {
		pvalues_col_copy[j].copy(pvalues_col[j]);
	}

	for (i = 0; i < row_size; i++) {
		if (pvalues_row_copy[i].count() == 0) {
			printf("NO SOLUTION!\n");
			return 0;
		}
	}


	bitvector rowmask(row_size);
	vector <In_Ex_Type> inexcluded(col_size, No_In_Ex);
	InExcludeSigs(pvalues_row_copy, pvalues_col_copy, cur_sol, excluded_sigs, included_sigs, inexcluded, rowmask, 1);

	vector<int> original_csum_sortindex;
	SortCol(pvalues_col_copy, original_csum_sortindex);

	//clock_t t2 = clock();
	//printf("time-read = %f seconds\n", ((float)(t2 - t1)) / CLOCKS_PER_SEC);

	while (rowmask.count() < row_size) {

		//printf(" rowmask-count = %i \n", rowmask.count());
		//printf("time-iter = %f seconds\n", ((float)(clock() - t2)) / CLOCKS_PER_SEC);
		//select the row with only a one, find the row has least one(except only a one case). Get remaining rows
		int min = col_size;
		vector<int> min_pos_list; //scan each row
		for (i = 0; i < row_size; i++) {
			if (pvalues_row_copy[i].count() == 1) {
				int pos = pvalues_row_copy[i].next_set(-1);  //get the selected column number
				cur_sol.push_back(pos);
			//	printf("SIG1 %i\n", pos);
				rowmask.or_count(pvalues_col_copy[pos]);  //mark the selected column
				RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, pos); //remove the covered row and selected col
				AdjustExSolutions(ex_sols, pos);  //insert the column into multiple solutions
															//min = 1;
				if ((rowmask.count() == row_size)) {
					goto PRINT_EXIT;
				}
			}
			else if (pvalues_row_copy[i].count() == 0);
			else if (pvalues_row_copy[i].count() < MINSIZE) {
				if (min > MINSIZE)
					min_pos_list.clear();
				min_pos_list.push_back(i);
				if (pvalues_row[i].count() < min)
					min = pvalues_row[i].count();   //min is the row that has least "one"
			}
			else if (pvalues_row_copy[i].count() == min) {
				min_pos_list.push_back(i);
			}
			else if (pvalues_row_copy[i].count() < min) {
				min_pos_list.clear();
				min_pos_list.push_back(i);
				min = pvalues_row_copy[i].count();
			}
		}

		// check the solution, if not found already!
		int max = 0;
		vector<int> max_pos_list; //the col cover most row
		vector<int> csum_sortindex;//rank the col from more one to less one
		for (k = 0; k < col_size; k++) {
			int cur = pvalues_col_copy[k].count();
			if (cur > 0) {
				csum_sortindex.push_back(k);
				int ll = csum_sortindex.size() - 2;
				while (ll >= 0) {  //sort the column again (some covered row and col has been clear)
					if (cur > pvalues_col_copy[csum_sortindex[ll]].count()) {
						csum_sortindex[ll + 1] = csum_sortindex[ll];
						ll--;
						if (ll == -1) {
							csum_sortindex[0] = k;
						}
					}
					else {
						csum_sortindex[ll + 1] = k;
						ll = -1;
					}
				}
			}
			if (cur > max) {  //find the column covered more row
				if (cur == (rowmask.size() - rowmask.count())) {
					cur_sol.push_back(k);
					if (IsExSolution(cur_sol, ex_sols)) {
						cur_sol.pop_back();
					}
					else {
					//	printf("SIG2 %i\n", k);
						goto PRINT_EXIT;
						//return 0;
					}
				}
				else {
					max = cur;
					max_pos_list.clear();
					max_pos_list.push_back(k);
				}
			}
			else if (cur == max) {
				max_pos_list.push_back(k);
			}
		}
		if (max_pos_list.size() == 0) {
			printf("NO SOLUTION!\n");
			return 0;
		}
	//	printf("max=%i - size=%i -- min = %i\n", max, (rowmask.size() - rowmask.count()), min);
		if (min_pos_list.size() > 0) {
			//max = 0;
			set<int> all_min_pos;
			//max_pos_list.clear();
			bitvector allminrows(col_size);
			bitvector allminrows_and(col_size);
			allminrows_and.setall();
			for (int mm = 0; mm < min_pos_list.size(); mm++) {
				allminrows.or_count(pvalues_row_copy[min_pos_list[mm]]);
				allminrows_and.and_count(pvalues_row_copy[min_pos_list[mm]]);
			}
			int nn = allminrows.next_set(-1);
			while (nn < col_size) {
				all_min_pos.insert(nn);
				if (pvalues_col_copy[nn].count() == row_size) {
					cur_sol.push_back(nn);
				//	printf("SIG3 %i\n", nn);
					goto PRINT_EXIT;
					//return 0;
				}

				nn = allminrows.next_set(nn);
			}
		//	printf("min_pos_list: %i  allminrows: %i(& %i)    all_min_pos: %i \n", min_pos_list.size(), allminrows.count(), allminrows_and.count(), all_min_pos.size());
			if (all_min_pos.size() > MAXMINSIZE) {
				vector<int> csum_sortindex_minlist;
				for (int kk = 0; kk < csum_sortindex.size(); kk++) {
					if (all_min_pos.find(csum_sortindex[kk]) != all_min_pos.end()) {
						csum_sortindex_minlist.push_back(csum_sortindex[kk]); //printf(" %i ", csum_sortindex[kk]);
						if (csum_sortindex_minlist.size() == all_min_pos.size())
							break;
					}
				}
				vector<int> best_sol;
				GetBest2Solution(pvalues_col_copy, rowmask, csum_sortindex_minlist, best_sol, ex_sols);
				if (best_sol.size() == 2) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
				//	printf("[[ 2 ]]\n");
					goto PRINT_EXIT;
				}
				GetBest2Solution(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);
				if (best_sol.size() == 2) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
				//	printf("[[[ 2 ]]]\n");
					goto PRINT_EXIT;
				}
				GetBest3Solution(pvalues_col_copy, rowmask, csum_sortindex_minlist, best_sol, ex_sols);
				if (best_sol.size() == 3) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
					cur_sol.push_back(best_sol[2]);
				//	printf("[[ 3 ]]\n");
					goto PRINT_EXIT;
				}
				//setcover::GetBest3Solution(pvalues_col_copy, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
				if (best_sol.size() == 3) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
					cur_sol.push_back(best_sol[2]);
				//	printf("[[[ 3 ]]]\n");
					goto PRINT_EXIT;
				}
				if (min < 4 && all_min_pos.size() > 4) { // select best 2!
					bitvector newrowmask;
					newrowmask.copy(allminrows);
					newrowmask.not_count();
					newrowmask.or_count(rowmask);
					GetBest2Selection(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);

					cur_sol.push_back(best_sol[0]);
				//	printf("SIG7_0 %i\n", best_sol[0]);
					rowmask.or_count(pvalues_col_copy[best_sol[0]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[0]);
					AdjustExSolutions(ex_sols, best_sol[0]);

					cur_sol.push_back(best_sol[1]);
				//	printf("SIG7_1 %i\n", best_sol[1]);
					rowmask.or_count(pvalues_col_copy[best_sol[1]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[1]);
					AdjustExSolutions(ex_sols, best_sol[1]);
				}
				else if (max_pos_list.size() > 0) {
					int loc = 0;
					while (loc < max_pos_list.size()) {
						if (all_min_pos.find(max_pos_list[loc]) != all_min_pos.end())
							break;
						else loc++;
					}
					if (loc >= max_pos_list.size())
						loc = 0;
					cur_sol.push_back(max_pos_list[loc]);
				//	printf("SIG6(%i) %i\n", loc, max_pos_list[loc]);
					rowmask.or_count(pvalues_col_copy[max_pos_list[loc]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[loc]);
					AdjustExSolutions(ex_sols, max_pos_list[loc]);
				}
				else {
					// restart to find another solution
					//cur_sol.pop_back();
					//if (cur_sol.size() > 0)
					//  goto START;
					return 0;
				}
			}
			else {
				vector<int> best_sol;
				GetBest2Solution(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);
				if (best_sol.size() == 2) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
				//	printf("<< 2 >>\n");
					goto PRINT_EXIT;
				}
				GetBest3Solution(pvalues_col_copy, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
				if (best_sol.size() == 3) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
					cur_sol.push_back(best_sol[2]);
				//	printf("<< 3 >>\n");
					goto PRINT_EXIT;
				}
				//setcover::GetBest4Solution(pvalues_col, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
				if (best_sol.size() == 4) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
					cur_sol.push_back(best_sol[2]);
					cur_sol.push_back(best_sol[3]);
				//	printf("<< 4 >>\n");
					goto PRINT_EXIT;
				}
				if (min < 4 && all_min_pos.size() > 4) { // select best 2!
					bitvector newrowmask;
					newrowmask.copy(allminrows);
					newrowmask.not_count();
					newrowmask.or_count(rowmask);
					GetBest2Selection(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);

					cur_sol.push_back(best_sol[0]);
				//	printf("SIG8_0 %i\n", best_sol[0]);
					rowmask.or_count(pvalues_col_copy[best_sol[0]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[0]);
					AdjustExSolutions(ex_sols, best_sol[0]);

					cur_sol.push_back(best_sol[1]);
				//	printf("SIG8_1 %i\n", best_sol[1]);
					rowmask.or_count(pvalues_col_copy[best_sol[1]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[1]);
					AdjustExSolutions(ex_sols, best_sol[1]);

					//if (cur_sol.size() > 3)
					//  setcover::OptimizeSolution2(pvalues_col, cur_sol);
					//if (cur_sol.size() > 5)
					//  setcover::OptimizeSolution3(pvalues_col, original_csum_sortindex, cur_sol);
					//if (cur_sol.size() > 3)
					//  setcover::OptimizeSolution2(pvalues_col, original_csum_sortindex, cur_sol);
				}
				else if (max_pos_list.size() > 0) {
					int loc = 0;
					while (loc < max_pos_list.size()) {
						if (all_min_pos.find(max_pos_list[loc]) != all_min_pos.end())
							break;
						else loc++;
					}
					if (loc >= max_pos_list.size())
						loc = 0;
					cur_sol.push_back(max_pos_list[loc]);
				//	printf("SIG4(%i) %i\n", loc, max_pos_list[loc]);
					rowmask.or_count(pvalues_col_copy[max_pos_list[loc]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[loc]);
					AdjustExSolutions(ex_sols, max_pos_list[loc]);
				}
				else {
					// restart to find another solution
					//cur_sol.pop_back();
					//if (cur_sol.size() > 0)
					//  goto START;
					return 0;
				}
			}
		}
		else {
			vector<int> best_sol;
			GetBest2Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);//********
			if (best_sol.size() == 2) {
				cur_sol.push_back(best_sol[0]);
				cur_sol.push_back(best_sol[1]);
			//	printf("(( 2 ))\n");
				goto PRINT_EXIT;
			}
			GetBest3Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);//********
			if (best_sol.size() == 3) {
				cur_sol.push_back(best_sol[0]);
				cur_sol.push_back(best_sol[1]);
				cur_sol.push_back(best_sol[2]);
			//	printf("(( 3 ))\n");
				goto PRINT_EXIT;
			}
			/*setcover::GetBest4Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);
			if (best_sol.size() == 4) {
			cur_sol.push_back(best_sol[0]);
			cur_sol.push_back(best_sol[1]);
			cur_sol.push_back(best_sol[2]);
			cur_sol.push_back(best_sol[3]);
			printf("(( 4 ))\n");
			goto PRINT_EXIT;
			}*/
			if (max_pos_list.size() > 0) {
				cur_sol.push_back(max_pos_list[0]);
			//	printf("SIG5(0) %i\n", max_pos_list[0]);
				rowmask.or_count(pvalues_col_copy[max_pos_list[0]]);
				RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[0]);
				AdjustExSolutions(ex_sols, max_pos_list[0]);
			}
			else {
				// restart to find another solution
				//cur_sol.pop_back();
				//if (cur_sol.size() > 0)
				//  goto START;
				return 0;
			}
		}
	}

PRINT_EXIT:
	//printf("time-iter = %f seconds\n", ((float)(clock() - t2)) / CLOCKS_PER_SEC);

	//if (cur_sol.size() > 3)
	if (cur_sol.size() >= 2 && cur_sol.size() <= 5) {
    SortWei(cur_sol);
    ReduceColCompress_All(pvalues_col, cur_sol, inexcluded);
  }
  else if (cur_sol.size() >= 6) {
    SortWei(cur_sol);
    ReduceColCompress(pvalues_col, cur_sol, inexcluded);
  }
  if (cur_sol.size() > 5) {
    SortWei(cur_sol);
    ReduceColReplace31(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);
  }

	//if (cur_sol.size() > 3)
	if (cur_sol.size() >= 2 && cur_sol.size() <= 5) {
    SortWei(cur_sol);
    ReduceColReplace21_Traverse(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);
  }
  else if (cur_sol.size() >= 6) {
    SortWei(cur_sol);
    ReduceColReplace21(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);
  }

	//printf("time-set covering without weight= %f seconds\n", ((float)(clock() - t1)) / CLOCKS_PER_SEC);

	//cout << "------Solution without considering weight:--------" << endl;
	printResult(cur_sol);

	for (vector <int>::iterator iter = cur_sol.begin(); iter != cur_sol.end(); iter++)
		selected_cands.push_back(signals[*iter]);

	cout << "-------------Setcovering Over(No weight)---------------" << endl;

	bitvector nomask(pvalues_col[0].size());
	if (!IsSolutionLite(pvalues_col, nomask, cur_sol)) {
		cout << "\n\n\n\n\n\n**********************(SC)Final solution Cannot cover all rows!!!!***************************\n\n\n\n\n\n" << endl;
		return 0;
	}

	return 1;
}


  int setcover::SolveMinCol(nodecircuit::NodeVector &selected_cands)
  {
  //  clock_t t1 = clock();

    int i, j, k;

    vector<int> cur_sol;

    cout << "----------------Setcovering Start(No weight)---------------" << endl;
    cout << "colsize=" << col_size << "  rowsize=" << row_size << endl;
  //  printf("\nPrevious solution and excluding positions...\n");
    vector<int> prev_sol;
    vector<int> ex_pos;
    set< set<int> > ex_sols;
    set<int> tmp_sol;
    j = 2;
    int p = 0;

    vector< bitvector > pvalues_row_copy, pvalues_col_copy;

    pvalues_row_copy.resize(row_size);
    for (i = 0; i < row_size; i++) {
      pvalues_row_copy[i].copy(pvalues_row[i]);
    }
    pvalues_col_copy.resize(col_size);
    for (j = 0; j < col_size; j++) {
      pvalues_col_copy[j].copy(pvalues_col[j]);
    }

    for (i = 0; i < row_size; i++) {
      if (pvalues_row_copy[i].count() == 0) {
        printf("NO SOLUTION!\n");
        return 0;
      }
    }


    bitvector rowmask(row_size);
    nodecircuit::NodeVector emptynodes;
    vector <In_Ex_Type> inexcluded(col_size, No_In_Ex);
    InExcludeSigs(pvalues_row_copy, pvalues_col_copy, cur_sol, emptynodes, emptynodes, inexcluded, rowmask, 1);

    //original_csum_sortindex--rank the column from more ones to less ones*************
    vector<int> original_csum_sortindex;
    SortCol(pvalues_col_copy, original_csum_sortindex);

  //  clock_t t2 = clock();
  //  printf("time-read = %f seconds\n", ((float)(t2 - t1)) / CLOCKS_PER_SEC);

    while (rowmask.count() < row_size) {

  //    printf(" rowmask-count = %i \n", rowmask.count());
  //    printf("time-iter = %f seconds\n", ((float)(clock() - t2)) / CLOCKS_PER_SEC);
      //select the row with only a one, find the row has least one(except only a one case). Get remaining rows
      int min = col_size;
      vector<int> min_pos_list; //scan each row
      for (i = 0; i < row_size; i++) {
        if (pvalues_row_copy[i].count() == 1) {
          int pos = pvalues_row_copy[i].next_set(-1);  //get the selected column number
          cur_sol.push_back(pos);
      //    printf("SIG1 %i\n", pos);
          rowmask.or_count(pvalues_col_copy[pos]);  //mark the selected column
          RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, pos); //remove the covered row and selected col
          AdjustExSolutions(ex_sols, pos);  //insert the column into multiple solutions
          //min = 1;
          if ((rowmask.count() == row_size)) {
            goto PRINT_EXIT;
          }
        }
        else if (pvalues_row_copy[i].count() == 0);
        else if (pvalues_row_copy[i].count() < MINSIZE) {
          if (min > MINSIZE)
            min_pos_list.clear();
          min_pos_list.push_back(i);
          if (pvalues_row_copy[i].count() < min)
            min = pvalues_row_copy[i].count();   //min is the row that has least "one"
        }
        else if (pvalues_row_copy[i].count() == min) {
          min_pos_list.push_back(i);
        }
        else if (pvalues_row_copy[i].count() < min) {
          min_pos_list.clear();
          min_pos_list.push_back(i);
          min = pvalues_row_copy[i].count();
        }
      }

      int max = 0;
      vector<int> max_pos_list;
      vector<int> csum_sortindex;
      for (k = 0; k < col_size; k++) {
        int cur = pvalues_col_copy[k].count();
        if (cur > 0) {
          csum_sortindex.push_back(k);
          int ll = csum_sortindex.size() - 2;
          while (ll >= 0) {
            if (cur > pvalues_col_copy[csum_sortindex[ll]].count()) {
              csum_sortindex[ll + 1] = csum_sortindex[ll];
              ll--;
              if (ll == -1) {
                csum_sortindex[0] = k;
              }
            }
            else {
              csum_sortindex[ll + 1] = k;
              ll = -1;
            }
          }
        }
        if (cur > max) {
          if (cur == (rowmask.size() - rowmask.count())) {
            cur_sol.push_back(k);
            if (IsExSolution(cur_sol, ex_sols)) {
              cur_sol.pop_back();
            }
            else {
          //    printf("SIG2 %i\n", k);
              goto PRINT_EXIT;
              //return 0;
            }
          }
          else {
            max = cur;
            max_pos_list.clear();
            max_pos_list.push_back(k);
          }
        }
        else if (cur == max) {
          max_pos_list.push_back(k);
        }
      }
      if (max_pos_list.size() == 0) {
    //    printf("NO SOLUTION!\n");
        return 0;
      }
    //  printf("max=%i - size=%i -- min = %i\n", max, (rowmask.size() - rowmask.count()), min);
      if (min_pos_list.size() > 0) {
        //max = 0;
        set<int> all_min_pos;
        //max_pos_list.clear();
        bitvector allminrows(col_size);
        bitvector allminrows_and(col_size);
        allminrows_and.setall();
        for (int mm = 0; mm < min_pos_list.size(); mm++) {
          allminrows.or_count(pvalues_row_copy[min_pos_list[mm]]);
          allminrows_and.and_count(pvalues_row_copy[min_pos_list[mm]]);
        }
        int nn = allminrows.next_set(-1);
        while (nn < col_size) {
          all_min_pos.insert(nn);
          if (pvalues_col_copy[nn].count() == row_size) {
            cur_sol.push_back(nn);
        //    printf("SIG3 %i\n", nn);
            goto PRINT_EXIT;
            //return 0;
          }

          nn = allminrows.next_set(nn);
        }
    //    printf("min_pos_list: %i  allminrows: %i(& %i)    all_min_pos: %i \n", min_pos_list.size(), allminrows.count(), allminrows_and.count(), all_min_pos.size());
        if (all_min_pos.size() > MAXMINSIZE) {
          vector<int> csum_sortindex_minlist;
          for (int kk = 0; kk < csum_sortindex.size(); kk++) {
            if (all_min_pos.find(csum_sortindex[kk]) != all_min_pos.end()) {
              csum_sortindex_minlist.push_back(csum_sortindex[kk]); //printf(" %i ", csum_sortindex[kk]);
              if (csum_sortindex_minlist.size() == all_min_pos.size())
                break;
            }
          }
          vector<int> best_sol;
          GetBest2Solution(pvalues_col_copy, rowmask, csum_sortindex_minlist, best_sol, ex_sols);
          if (best_sol.size() == 2) {
            cur_sol.push_back(best_sol[0]);
            cur_sol.push_back(best_sol[1]);
        //    printf("[[ 2 ]]\n");
            goto PRINT_EXIT;
          }
          GetBest2Solution(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);
          if (best_sol.size() == 2) {
            cur_sol.push_back(best_sol[0]);
            cur_sol.push_back(best_sol[1]);
        //    printf("[[[ 2 ]]]\n");
            goto PRINT_EXIT;
          }
          GetBest3Solution(pvalues_col_copy, rowmask, csum_sortindex_minlist, best_sol, ex_sols);
          if (best_sol.size() == 3) {
            cur_sol.push_back(best_sol[0]);
            cur_sol.push_back(best_sol[1]);
            cur_sol.push_back(best_sol[2]);
        //    printf("[[ 3 ]]\n");
            goto PRINT_EXIT;
          }
          //setcover::GetBest3Solution(pvalues_col_copy, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
          if (best_sol.size() == 3) {
            cur_sol.push_back(best_sol[0]);
            cur_sol.push_back(best_sol[1]);
            cur_sol.push_back(best_sol[2]);
          //  printf("[[[ 3 ]]]\n");
            goto PRINT_EXIT;
          }
          if (min < 4 && all_min_pos.size() > 4) { // select best 2!
            bitvector newrowmask;
            newrowmask.copy(allminrows);
            newrowmask.not_count();
            newrowmask.or_count(rowmask);
            GetBest2Selection(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);

            cur_sol.push_back(best_sol[0]);
        //    printf("SIG7_0 %i\n", best_sol[0]);
            rowmask.or_count(pvalues_col_copy[best_sol[0]]);
            RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[0]);
            AdjustExSolutions(ex_sols, best_sol[0]);

            cur_sol.push_back(best_sol[1]);
        //    printf("SIG7_1 %i\n", best_sol[1]);
            rowmask.or_count(pvalues_col_copy[best_sol[1]]);
            RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[1]);
            AdjustExSolutions(ex_sols, best_sol[1]);
          }
          else if (max_pos_list.size() > 0) {
            int loc = 0;
            while (loc < max_pos_list.size()) {
              if (all_min_pos.find(max_pos_list[loc]) != all_min_pos.end())
                break;
              else loc++;
            }
            if (loc >= max_pos_list.size())
              loc = 0;
            cur_sol.push_back(max_pos_list[loc]);
        //    printf("SIG6(%i) %i\n", loc, max_pos_list[loc]);
            rowmask.or_count(pvalues_col_copy[max_pos_list[loc]]);
            RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[loc]);
            AdjustExSolutions(ex_sols, max_pos_list[loc]);
          }
          else {
            // restart to find another solution
            //cur_sol.pop_back();
            //if (cur_sol.size() > 0)
            //  goto START;
            return 0;
          }
        }
        else {
          vector<int> best_sol;
          GetBest2Solution(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);
          if (best_sol.size() == 2) {
            cur_sol.push_back(best_sol[0]);
            cur_sol.push_back(best_sol[1]);
      //      printf("<< 2 >>\n");
            goto PRINT_EXIT;
          }
          GetBest3Solution(pvalues_col_copy, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
          if (best_sol.size() == 3) {
            cur_sol.push_back(best_sol[0]);
            cur_sol.push_back(best_sol[1]);
            cur_sol.push_back(best_sol[2]);
        //    printf("<< 3 >>\n");
            goto PRINT_EXIT;
          }
          //setcover::GetBest4Solution(pvalues_col, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
          if (best_sol.size() == 4) {
            cur_sol.push_back(best_sol[0]);
            cur_sol.push_back(best_sol[1]);
            cur_sol.push_back(best_sol[2]);
            cur_sol.push_back(best_sol[3]);
        //    printf("<< 4 >>\n");
            goto PRINT_EXIT;
          }
          if (min < 4 && all_min_pos.size() > 4) { // select best 2!
            bitvector newrowmask;
            newrowmask.copy(allminrows);
            newrowmask.not_count();
            newrowmask.or_count(rowmask);
            GetBest2Selection(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);

            cur_sol.push_back(best_sol[0]);
        //    printf("SIG8_0 %i\n", best_sol[0]);
            rowmask.or_count(pvalues_col_copy[best_sol[0]]);
            RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[0]);
            AdjustExSolutions(ex_sols, best_sol[0]);

            cur_sol.push_back(best_sol[1]);
        //    printf("SIG8_1 %i\n", best_sol[1]);
            rowmask.or_count(pvalues_col_copy[best_sol[1]]);
            RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[1]);
            AdjustExSolutions(ex_sols, best_sol[1]);

            //if (cur_sol.size() > 3)
            //  setcover::OptimizeSolution2(pvalues_col, cur_sol);
            //if (cur_sol.size() > 5)
            //  setcover::OptimizeSolution3(pvalues_col, original_csum_sortindex, cur_sol);
            //if (cur_sol.size() > 3)
            //  setcover::OptimizeSolution2(pvalues_col, original_csum_sortindex, cur_sol);
          }
          else if (max_pos_list.size() > 0) {
            int loc = 0;
            while (loc < max_pos_list.size()) {
              if (all_min_pos.find(max_pos_list[loc]) != all_min_pos.end())
                break;
              else loc++;
            }
            if (loc >= max_pos_list.size())
              loc = 0;
            cur_sol.push_back(max_pos_list[loc]);
        //    printf("SIG4(%i) %i\n", loc, max_pos_list[loc]);
            rowmask.or_count(pvalues_col_copy[max_pos_list[loc]]);
            RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[loc]);
            AdjustExSolutions(ex_sols, max_pos_list[loc]);
          }
          else {
            // restart to find another solution
            //cur_sol.pop_back();
            //if (cur_sol.size() > 0)
            //  goto START;
            return 0;
          }
        }
      }
      else {
        vector<int> best_sol;
        GetBest2Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);//********
        if (best_sol.size() == 2) {
          cur_sol.push_back(best_sol[0]);
          cur_sol.push_back(best_sol[1]);
    //      printf("(( 2 ))\n");
          goto PRINT_EXIT;
        }
        GetBest3Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);//********
        if (best_sol.size() == 3) {
          cur_sol.push_back(best_sol[0]);
          cur_sol.push_back(best_sol[1]);
          cur_sol.push_back(best_sol[2]);
      //    printf("(( 3 ))\n");
          goto PRINT_EXIT;
        }
        /*setcover::GetBest4Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);
        if (best_sol.size() == 4) {
        cur_sol.push_back(best_sol[0]);
        cur_sol.push_back(best_sol[1]);
        cur_sol.push_back(best_sol[2]);
        cur_sol.push_back(best_sol[3]);
        printf("(( 4 ))\n");
        goto PRINT_EXIT;
        }*/
        if (max_pos_list.size() > 0) {
          cur_sol.push_back(max_pos_list[0]);
      //    printf("SIG5(0) %i\n", max_pos_list[0]);
          rowmask.or_count(pvalues_col_copy[max_pos_list[0]]);
          RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[0]);
          AdjustExSolutions(ex_sols, max_pos_list[0]);
        }
        else {
          // restart to find another solution
          //cur_sol.pop_back();
          //if (cur_sol.size() > 0)
          //  goto START;
          return 0;
        }
      }
    }

    PRINT_EXIT:
//    printf("time-iter = %f seconds\n", ((float)(clock() - t2)) / CLOCKS_PER_SEC);

    //if (cur_sol.size() > 3)
    if (cur_sol.size() >= 2 && cur_sol.size() <= 5)
      ReduceColCompress_All(pvalues_col, cur_sol, inexcluded);
    else if (cur_sol.size() >= 6)
      ReduceColCompress(pvalues_col, cur_sol, inexcluded);

    if (cur_sol.size() > 5)
      ReduceColReplace31(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);

    //if (cur_sol.size() > 3)
    if (cur_sol.size() >= 2 && cur_sol.size() <= 5)
      ReduceColReplace21_Traverse(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);
    else if (cur_sol.size() >= 6)
      ReduceColReplace21(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);


    cout << "------Solution without considering weight:--------" << endl;
    printResult(cur_sol);

    for (vector <int>::iterator iter = cur_sol.begin(); iter != cur_sol.end(); iter++)
      selected_cands.push_back(signals[*iter]);

    cout << "-------------Setcovering Over(No weight)------------" << endl;

    bitvector nomask(pvalues_col[0].size());
    if (!IsSolutionLite(pvalues_col, nomask, cur_sol)) {
      cout << "\n\n\n\n\n\n**********************(SC)Final solution Cannot cover all rows!!!!***************************\n\n\n\n\n\n" << endl;
      return 0;
    }

    return 1;
  }

	int setcover::PartitionWei(vector <int> &sol, int low, int high){
	    int pivot=sol[low];
	    while(low<high){
	        while(low<high && signals[sol[high]]->weight<=signals[pivot]->weight)
	            high--;
	        sol[low]=sol[high];
	        while(low<high && signals[sol[low]]->weight>=signals[pivot]->weight)
	            low++;
	        sol[high]=sol[low];
	    }
	    sol[low]=pivot;
	    return low;
	}


int setcover::SortWei(vector <int> &sol) {

	vector <int> pivots;
	int low,high,mid;
	low=0;
	high=sol.size()-1;
	mid=PartitionWei(sol,low,high);
	if(low<mid-1){
			pivots.push_back(low);
			pivots.push_back(mid-1);
	}
	if(high>mid+1){
			pivots.push_back(mid+1);
			pivots.push_back(high);
	}

	while(pivots.size()){
			high=pivots[pivots.size()-1];
			pivots.pop_back();
			low=pivots[pivots.size()-1];
			pivots.pop_back();
			mid=PartitionWei(sol,low,high);
			if(low<mid-1){
					pivots.push_back(low);
					pivots.push_back(mid-1);
			}
			if(high>mid+1){
					pivots.push_back(mid+1);
					pivots.push_back(high);
			}
	}


/*  for (int i = 1; i < sol.size(); i++) {
    int soli = sol[i];
    int j = i-1;
    while (j >= 0 && signals[soli].weight > signals[sol[j]].weight) {
      sol[j+1] = sol[j];
      j--;
    }
    sol[j+1] = soli;
  }

	int i = 0;
	vector <int>::iterator iter;
	vector <int> tmp;
	tmp.push_back(sol[0]);
	for (i = 1;i < sol.size();i++) {
		for (iter = tmp.begin();iter != tmp.end();iter++) {
			if (signals[sol[i]].weight > signals[(*iter)].weight) {
				tmp.insert(iter,sol[i]);
				break;
			}
		}
		if(iter == tmp.end())
			tmp.insert(iter, sol[i]); //smallest number at last
	}
	sol.clear();
	sol.assign(tmp.begin(),tmp.end());
*/
	return 1;
}

int setcover::ReduceWeightReplace11(vector <In_Ex_Type> &inexcluded, vector< bitvector > &pvalues_col, vector<int> &cur_sol)
{
	int cur_weight = 0;
	for (unsigned int i = 0; i < cur_sol.size(); i++)
		cur_weight = cur_weight + signals[cur_sol[i]]->weight;

	for (int cur_sig = 0; cur_sig < cur_sol.size(); cur_sig++)
	{
		{
			bitvector curcol;
			curcol.copy(pvalues_col[cur_sol[cur_sig]]);
			int curcnt = curcol.count();
			for (int m = 0; m < cur_sol.size(); m++) {
				if (m != cur_sig)
					curcnt = curcol.mask_count(pvalues_col[cur_sol[m]]);
			}
			int cur_col_wei = signals[cur_sol[cur_sig]]->weight;

			for(int index = 0; signals[index]->weight < cur_col_wei && index < col_size; index++) {

				if (pvalues_col[index].count() >= curcnt && inexcluded[index] != ExcludedFlag) {
					if (find(cur_sol.begin(), cur_sol.end(), index) != cur_sol.end())  //avoid the repetition.
						continue;
					if (count_and(curcol, pvalues_col[index]) == curcnt) {
						cur_sol[cur_sig] = index;
                        SortWei(cur_sol);
						cur_weight = cur_weight - cur_col_wei + signals[index]->weight;
						cur_sig = -1;
						break;
					}
				}
			}
		}
	}

	return cur_weight;
}

int setcover::ReduceWeightReplace11_Traverse(vector <In_Ex_Type> &inexcluded, vector< bitvector > &pvalues_col, vector<int> &cur_sol)
{
    SortWei(cur_sol);
	int presol_weight = 0;
	for (unsigned int i = 0; i < cur_sol.size(); i++)
		presol_weight = presol_weight + signals[cur_sol[i]]->weight;
	int best_weight = presol_weight;
	vector<int> best_sol;

	for (int start_sig = 0; start_sig < cur_sol.size();start_sig++)
	{
		vector<int>tmp_sol(cur_sol.size());

		for (int i = start_sig;i < cur_sol.size();i++)
			tmp_sol[i - start_sig] = cur_sol[i];
		for (int i = 0;i < start_sig;i++)
			tmp_sol[cur_sol.size() - start_sig + i] = cur_sol[i];

		int new_weight = ReduceWeightReplace11(inexcluded, pvalues_col, tmp_sol);
		if (best_weight > new_weight)
		{
			best_weight = new_weight;
			best_sol.clear();
			best_sol.assign(tmp_sol.begin(), tmp_sol.end());
		}
	}

	if (best_weight < presol_weight)
	{
		cur_sol.clear();
		cur_sol.assign(best_sol.begin(), best_sol.end());
		cout << "------Traverse final new sol(WeightRe11)-------" << endl;
		printResult(cur_sol);
	}

	return best_weight;
}

int setcover::ReduceWeightReplace22(vector< bitvector > &pvalues_col, vector<int> &csum_sortindex, vector<int> &sol, vector <In_Ex_Type> &inexcluded, const int t_limit) {

    SortWei(sol);

    clock_t wei_reduce22_start = clock();

    int pre_weight = 0;
    for (unsigned int i = 0; i < sol.size(); i++)
        pre_weight = pre_weight + signals[sol[i]]->weight;

    for (int i = 0; i < sol.size() - 1; i++){
        for (int j = i + 1; j < sol.size(); j++){
            {
                bitvector both;
                both.copy(pvalues_col[sol[i]]);
                int cnt = both.or_count(pvalues_col[sol[j]]);
                for (int m = 0; m < sol.size(); m++){
                    if (m != i && m != j)
                        cnt = both.mask_count(pvalues_col[sol[m]]);
                }

                int k = 0;
                //Only check the columns of count/2.
                //while (k < csum_sortindex.size() && pvalues_col[csum_sortindex[k]].count() >= cnt / 2)
                //    k++;

                for (int c1 = 0;c1 < col_size - 2;c1++) {
                    for (int c2 = c1 + 1;c2 < col_size - 1;c2++) {
                        //-----------Limit the execution time------------
                        clock_t wei_reduce22_cur = clock();
                        if ((wei_reduce22_cur - wei_reduce22_start) / CLOCKS_PER_SEC > t_limit) {
                            cout << "\n*****WeightRe22 time out, stop and return******\n" << endl;
                            return 1;
                        }
                        //-----------------------------------------------
                        //int newcol1 = csum_sortindex[c1];
                        //int newcol2 = csum_sortindex[c2];
                        int newcol1 = c1;
                        int newcol2 = c2;
                        if (find(sol.begin(), sol.end(), newcol1) != sol.end() && find(sol.begin(), sol.end(), newcol2) != sol.end())
                            continue;
                        if (inexcluded[newcol1] != ExcludedFlag && inexcluded[newcol2] != ExcludedFlag &&
                            pvalues_col[newcol1].count() + pvalues_col[newcol2].count() >= cnt) {
                            if (signals[sol[i]]->weight + signals[sol[j]]->weight > signals[newcol1]->weight + signals[newcol2]->weight){
                                bitvector both2;
                                both2.copy(pvalues_col[newcol1]);
                                both2.or_count(pvalues_col[newcol2]);
                                int cnt2 = count_and(both, both2);
                                if (cnt2 == cnt)
                                {
                                    sol[i] = newcol1;
                                    sol[j] = newcol2;
                                    SortWei(sol);
                                    j = sol.size() + 2;
                                    i = -1;
                                    goto NextLoop;
                                }
                            }
                        }
                    }
                }
            }
        NextLoop:;
        }
    }

    int cur_weight = 0;
    for (unsigned int i = 0; i < sol.size(); i++)
        cur_weight = cur_weight + signals[sol[i]]->weight;
    if(cur_weight < pre_weight) {
        cout << "----new col(Replace22)----" << endl;
        printResult(sol);
    }
    return 1;
}


int setcover::OptimizeWeight(nodecircuit::NodeVector &current_cands, nodecircuit::NodeVector &new_cands, nodecircuit::NodeVector &excluded_sigs, nodecircuit::NodeVector &included_sigs, const int t_limit)
{
  new_cands = current_cands; // AMIR
	cout << "\n\n\n\n-----------------------------Weighted Setcovering Start-----------------------------\n\n" << endl;
    cout << "colsize=" << col_size << "  rowsize=" << row_size << "\n" << endl;
	//clock_t t_wei_start = clock();
	if (!current_cands.size()) {
		cout << "********No candidate here!!**********" << endl;
		return 0;
	}

	int i, j, k;
	vector <int> cur_sol;
	for (int col = 0;col < col_size;col++) {
		if (cur_sol.size() < current_cands.size() && signals[col]->weight) {
			nodecircuit::NodeVector::iterator iter = find(current_cands.begin(), current_cands.end(), signals[col]);
			if (iter != current_cands.end()) {
				cur_sol.push_back(col);
			}
		}
		if (cur_sol.size() == current_cands.size())
			break;
	}

	bitvector nomask(pvalues_col[0].size());
	if (!IsSolutionLite(pvalues_col, nomask, cur_sol)) {
		cout << "\n\n\n\n\n\n**********************Input solution Cannot cover all rows!!!!***************************\n\n\n\n\n\n" << endl;
		return 0;
	}

	vector< bitvector > pvalues_row_copy, pvalues_col_copy;
	pvalues_row_copy.resize(row_size);
	for (i = 0; i < row_size; i++) {
		pvalues_row_copy[i].copy(pvalues_row[i]);
	}
	pvalues_col_copy.resize(col_size);
	/*for (j = 0; j < col_size; j++) {
		pvalues_col_copy[j].copy(pvalues_col[j]);
	}*/
	bitvector rowmask(row_size);
	vector <In_Ex_Type> inexcluded(col_size, No_In_Ex);
	InExcludeSigs(pvalues_row_copy, pvalues_col_copy, cur_sol, excluded_sigs, included_sigs, inexcluded, rowmask, 0);

	vector<int> original_csum_sortindex;
//	SortCol(pvalues_col, original_csum_sortindex);

	//----------------weight--------------------
	ReduceWeightReplace11_Traverse(inexcluded, pvalues_col, cur_sol);
	//***********set time limit*************** 1s < limit < 600s
	int t_limit_in = 0;
	if (t_limit > 600)
		t_limit_in = 600;
	else if (t_limit < 1)
		t_limit_in = 1;
	else
		t_limit_in = t_limit;
	cout << "\n----------Time limit for WeightRe22 is: " << t_limit_in << " seconds----------"<< endl;
	// AMIR
  //if (cur_sol.size() >= 2)
	//	ReduceWeightReplace22(pvalues_col, original_csum_sortindex, cur_sol, inexcluded, t_limit_in);
	//------------------------------------------

  //AMIR
  int original_weight = 0;
  for (int cnt = 0; cnt < current_cands.size(); cnt++)
    original_weight += current_cands[cnt]->weight;

  int new_weight = 0;
	new_cands.clear();
	for (vector <int>::iterator iter = cur_sol.begin(); iter != cur_sol.end(); iter++) {
		new_cands.push_back(signals[*iter]);
    new_weight += new_cands.back()->weight;
  }

  //AMIR . revert back to original if not good!
  if (new_weight > original_weight || (new_weight == original_weight && new_cands.size() >= current_cands.size()))
    new_cands = current_cands;

	cout << "\n--Final solution after optimization of weight--" << endl;
	printResult(cur_sol);
//	solutionVerify(cur_sol);
	//printf("time-set reduced weight= %f seconds\n", ((float)(clock() - t_wei_start)) / CLOCKS_PER_SEC);
	cout << "--------------Weighted Setcovering Over---------------" << endl;

	if (!IsSolutionLite(pvalues_col, nomask, cur_sol)) {
		cout << "\n\n\n\n\n\n**********************(WSC)Final solution Cannot cover all rows!!!!***************************\n\n\n\n\n\n" << endl;
		new_cands = current_cands;
		return 0;
	}

	return 1;
}

int setcover::InExcludeSigs_Range(vector< bitvector > &pvalues_row_copy, vector< bitvector > &pvalues_col_copy, vector <int> &cur_sol, int ex_col_start, int ex_col_end, vector <In_Ex_Type> &inexcluded, int wri_pvalue)
{
	if (ex_col_start > ex_col_end) {
		cout << "*********Warning: Exclude range start should not be bigger than end*********" << endl;
		return 0;
	}
	if (ex_col_end >= col_size) {
		cout << "*********Warning: Exclude range end should not be bigger than column number*********" << endl;
		ex_col_end = col_size-1;
		//return 0;
	}
	if (ex_col_start < 0) {
		cout << "*********Warning: Exclude range end should not be bigger than column number*********" << endl;
		ex_col_start = 0;
		//return 0;
	}

	cout << "Exclude col range: " << ex_col_start << "--" << ex_col_end << endl;

	//Find the columns of excluded and included signals
	for (int col = ex_col_start; col <= ex_col_end; col++)
		inexcluded[col] = ExcludedFlag;

//	if (wri_pvalue)
//	{
		for (int col = ex_col_start;col <= ex_col_end; col++) {
			pvalues_col_copy[col].clear();
			for (int j = 0;j < row_size;j++)
				pvalues_row_copy[j].reset(col);
		}
		for (int i = 0; i < row_size; i++) {
			if (pvalues_row_copy[i].count() == 0) {
				cout << "*********Warning: NO SOLUTION if these columns are excluded!*********" << endl;
				return 0;
			}
		}
//	}
		for (vector <int>::iterator iter = cur_sol.begin();iter != cur_sol.end();iter++)
			if (inexcluded[*iter] == ExcludedFlag)
				cout << "***********Warning: the input solution " << signals[*iter]->name << "(Col " << *iter << ") " << "is inside the excluding range*********" << endl;

	return 1;
}

int setcover::SolveMinCol_ExRange(nodecircuit::NodeVector &selected_cands, int ex_col_start, int ex_col_end)
{
	//clock_t t1 = clock();

	int i, j, k;

	vector<int> cur_sol;

	cout << "----------------Setcovering Start(No weight)---------------" << endl;
	cout << "colsize=" << col_size << "  rowsize=" << row_size << endl;
	vector<int> prev_sol;
	vector<int> ex_pos;
	set< set<int> > ex_sols;
	set<int> tmp_sol;
	j = 2;
	int p = 0;

	vector< bitvector > pvalues_row_copy, pvalues_col_copy;

	pvalues_row_copy.resize(row_size);
	for (i = 0; i < row_size; i++) {
		pvalues_row_copy[i].copy(pvalues_row[i]);
	}
	pvalues_col_copy.resize(col_size);
	for (j = 0; j < col_size; j++) {
		pvalues_col_copy[j].copy(pvalues_col[j]);
	}

	for (i = 0; i < row_size; i++) {
		if (pvalues_row_copy[i].count() == 0) {
			printf("NO SOLUTION!\n");
			return 0;
		}
	}


	bitvector rowmask(row_size);
	vector <In_Ex_Type> inexcluded(col_size, No_In_Ex);
	int ex_ret = InExcludeSigs_Range(pvalues_row_copy, pvalues_col_copy, cur_sol, ex_col_start, ex_col_end, inexcluded, 1);
  if (!ex_ret)
    return 0;



	//original_csum_sortindex--rank the column from more ones to less ones*************
	vector<int> original_csum_sortindex;
	SortCol(pvalues_col_copy, original_csum_sortindex);

	//clock_t t2 = clock();
	//printf("time-read = %f seconds\n", ((float)(t2 - t1)) / CLOCKS_PER_SEC);

	while (rowmask.count() < row_size) {


		int min = col_size;
		vector<int> min_pos_list;
		for (i = 0; i < row_size; i++) {
			if (pvalues_row_copy[i].count() == 1) {
				int pos = pvalues_row_copy[i].next_set(-1);
				cur_sol.push_back(pos);
			//	printf("SIG1 %i\n", pos);
				rowmask.or_count(pvalues_col_copy[pos]);
				RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, pos);
				AdjustExSolutions(ex_sols, pos);
															//min = 1;
				if ((rowmask.count() == row_size)) {
					goto PRINT_EXIT;
				}
			}
			else if (pvalues_row_copy[i].count() == 0);
			else if (pvalues_row_copy[i].count() < MINSIZE) {
				if (min > MINSIZE)
					min_pos_list.clear();
				min_pos_list.push_back(i);
				if (pvalues_row_copy[i].count() < min)
					min = pvalues_row_copy[i].count();
			}
			else if (pvalues_row_copy[i].count() == min) {
				min_pos_list.push_back(i);
			}
			else if (pvalues_row_copy[i].count() < min) {
				min_pos_list.clear();
				min_pos_list.push_back(i);
				min = pvalues_row_copy[i].count();
			}
		}

		// check the solution, if not found already!
		int max = 0;
		vector<int> max_pos_list;
		vector<int> csum_sortindex;
		for (k = 0; k < col_size; k++) {
			int cur = pvalues_col_copy[k].count();
			if (cur > 0) {
				csum_sortindex.push_back(k);
				int ll = csum_sortindex.size() - 2;
				while (ll >= 0) {
					if (cur > pvalues_col_copy[csum_sortindex[ll]].count()) {
						csum_sortindex[ll + 1] = csum_sortindex[ll];
						ll--;
						if (ll == -1) {
							csum_sortindex[0] = k;
						}
					}
					else {
						csum_sortindex[ll + 1] = k;
						ll = -1;
					}
				}
			}
			if (cur > max) {
				if (cur == (rowmask.size() - rowmask.count())) {
					cur_sol.push_back(k);
					if (IsExSolution(cur_sol, ex_sols)) {
						cur_sol.pop_back();
					}
					else {
				//		printf("SIG2 %i\n", k);
						goto PRINT_EXIT;
						//return 0;
					}
				}
				else {
					max = cur;
					max_pos_list.clear();
					max_pos_list.push_back(k);
				}
			}
			else if (cur == max) {
				max_pos_list.push_back(k);
			}
		}
		if (max_pos_list.size() == 0) {
			printf("NO SOLUTION!\n");
			return 0;
		}
	//	printf("max=%i - size=%i -- min = %i\n", max, (rowmask.size() - rowmask.count()), min);
		if (min_pos_list.size() > 0) {
			//max = 0;
			set<int> all_min_pos;
			//max_pos_list.clear();
			bitvector allminrows(col_size);
			bitvector allminrows_and(col_size);
			allminrows_and.setall();
			for (int mm = 0; mm < min_pos_list.size(); mm++) {
				allminrows.or_count(pvalues_row_copy[min_pos_list[mm]]);
				allminrows_and.and_count(pvalues_row_copy[min_pos_list[mm]]);
			}
			int nn = allminrows.next_set(-1);
			while (nn < col_size) {
				all_min_pos.insert(nn);
				if (pvalues_col_copy[nn].count() == row_size) {
					cur_sol.push_back(nn);
				//	printf("SIG3 %i\n", nn);
					goto PRINT_EXIT;
					//return 0;
				}
				/*if (pvalues_col[nn].count() > max) {
				if (pvalues_col[nn].count() == row_size) {
				cur_sol.push_back(nn);
				printf("SIG3 %i\n", nn);
				goto PRINT_EXIT;
				//return 0;
				}
				else {
				max = pvalues_col[nn].count();
				max_pos_list.clear();
				max_pos_list.push_back(nn);
				}
				}
				else if (pvalues_col[nn].count() == max) {
				max_pos_list.push_back(nn);
				}*/
				nn = allminrows.next_set(nn);
			}
		//	printf("min_pos_list: %i  allminrows: %i(& %i)    all_min_pos: %i \n", min_pos_list.size(), allminrows.count(), allminrows_and.count(), all_min_pos.size());
			if (all_min_pos.size() > MAXMINSIZE) {
				vector<int> csum_sortindex_minlist;
				for (int kk = 0; kk < csum_sortindex.size(); kk++) {
					if (all_min_pos.find(csum_sortindex[kk]) != all_min_pos.end()) {
						csum_sortindex_minlist.push_back(csum_sortindex[kk]); //printf(" %i ", csum_sortindex[kk]);
						if (csum_sortindex_minlist.size() == all_min_pos.size())
							break;
					}
				}
				vector<int> best_sol;
				GetBest2Solution(pvalues_col_copy, rowmask, csum_sortindex_minlist, best_sol, ex_sols);
				if (best_sol.size() == 2) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
			//		printf("[[ 2 ]]\n");
					goto PRINT_EXIT;
				}
				GetBest2Solution(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);
				if (best_sol.size() == 2) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
			//		printf("[[[ 2 ]]]\n");
					goto PRINT_EXIT;
				}
				GetBest3Solution(pvalues_col_copy, rowmask, csum_sortindex_minlist, best_sol, ex_sols);
				if (best_sol.size() == 3) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
					cur_sol.push_back(best_sol[2]);
		//			printf("[[ 3 ]]\n");
					goto PRINT_EXIT;
				}
				//setcover::GetBest3Solution(pvalues_col_copy, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
				if (best_sol.size() == 3) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
					cur_sol.push_back(best_sol[2]);
		//			printf("[[[ 3 ]]]\n");
					goto PRINT_EXIT;
				}
				if (min < 4 && all_min_pos.size() > 4) { // select best 2!
					bitvector newrowmask;
					newrowmask.copy(allminrows);
					newrowmask.not_count();
					newrowmask.or_count(rowmask);
					GetBest2Selection(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);

					cur_sol.push_back(best_sol[0]);
			//		printf("SIG7_0 %i\n", best_sol[0]);
					rowmask.or_count(pvalues_col_copy[best_sol[0]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[0]);
					AdjustExSolutions(ex_sols, best_sol[0]);

					cur_sol.push_back(best_sol[1]);
		//			printf("SIG7_1 %i\n", best_sol[1]);
					rowmask.or_count(pvalues_col_copy[best_sol[1]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[1]);
					AdjustExSolutions(ex_sols, best_sol[1]);

					//if (cur_sol.size() > 3)
					//  setcover::OptimizeSolution2(pvalues_col, cur_sol);
					//if (cur_sol.size() > 5)
					//  setcover::OptimizeSolution3(pvalues_col, original_csum_sortindex, cur_sol);
					//if (cur_sol.size() > 3)
					//  setcover::OptimizeSolution2(pvalues_col, original_csum_sortindex, cur_sol);
				}
				else if (max_pos_list.size() > 0) {
					int loc = 0;
					while (loc < max_pos_list.size()) {
						if (all_min_pos.find(max_pos_list[loc]) != all_min_pos.end())
							break;
						else loc++;
					}
					if (loc >= max_pos_list.size())
						loc = 0;
					cur_sol.push_back(max_pos_list[loc]);
		//			printf("SIG6(%i) %i\n", loc, max_pos_list[loc]);
					rowmask.or_count(pvalues_col_copy[max_pos_list[loc]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[loc]);
					AdjustExSolutions(ex_sols, max_pos_list[loc]);
				}
				else {
					// restart to find another solution
					//cur_sol.pop_back();
					//if (cur_sol.size() > 0)
					//  goto START;
					return 0;
				}
			}
			else {
				vector<int> best_sol;
				GetBest2Solution(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);
				if (best_sol.size() == 2) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
		//			printf("<< 2 >>\n");
					goto PRINT_EXIT;
				}
				GetBest3Solution(pvalues_col_copy, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
				if (best_sol.size() == 3) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
					cur_sol.push_back(best_sol[2]);
		//			printf("<< 3 >>\n");
					goto PRINT_EXIT;
				}
				//setcover::GetBest4Solution(pvalues_col, rowmask, all_min_pos, csum_sortindex, best_sol, ex_sols);
				if (best_sol.size() == 4) {
					cur_sol.push_back(best_sol[0]);
					cur_sol.push_back(best_sol[1]);
					cur_sol.push_back(best_sol[2]);
					cur_sol.push_back(best_sol[3]);
		//			printf("<< 4 >>\n");
					goto PRINT_EXIT;
				}
				if (min < 4 && all_min_pos.size() > 4) { // select best 2!
					bitvector newrowmask;
					newrowmask.copy(allminrows);
					newrowmask.not_count();
					newrowmask.or_count(rowmask);
					GetBest2Selection(pvalues_col_copy, rowmask, all_min_pos, best_sol, ex_sols);

					cur_sol.push_back(best_sol[0]);
		//			printf("SIG8_0 %i\n", best_sol[0]);
					rowmask.or_count(pvalues_col_copy[best_sol[0]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[0]);
					AdjustExSolutions(ex_sols, best_sol[0]);

					cur_sol.push_back(best_sol[1]);
			//		printf("SIG8_1 %i\n", best_sol[1]);
					rowmask.or_count(pvalues_col_copy[best_sol[1]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, best_sol[1]);
					AdjustExSolutions(ex_sols, best_sol[1]);

					//if (cur_sol.size() > 3)
					//  setcover::OptimizeSolution2(pvalues_col, cur_sol);
					//if (cur_sol.size() > 5)
					//  setcover::OptimizeSolution3(pvalues_col, original_csum_sortindex, cur_sol);
					//if (cur_sol.size() > 3)
					//  setcover::OptimizeSolution2(pvalues_col, original_csum_sortindex, cur_sol);
				}
				else if (max_pos_list.size() > 0) {
					int loc = 0;
					while (loc < max_pos_list.size()) {
						if (all_min_pos.find(max_pos_list[loc]) != all_min_pos.end())
							break;
						else loc++;
					}
					if (loc >= max_pos_list.size())
						loc = 0;
					cur_sol.push_back(max_pos_list[loc]);
			//		printf("SIG4(%i) %i\n", loc, max_pos_list[loc]);
					rowmask.or_count(pvalues_col_copy[max_pos_list[loc]]);
					RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[loc]);
					AdjustExSolutions(ex_sols, max_pos_list[loc]);
				}
				else {
					// restart to find another solution
					//cur_sol.pop_back();
					//if (cur_sol.size() > 0)
					//  goto START;
					return 0;
				}
			}
		}
		else {
			vector<int> best_sol;
			GetBest2Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);//********
			if (best_sol.size() == 2) {
				cur_sol.push_back(best_sol[0]);
				cur_sol.push_back(best_sol[1]);
		//		printf("(( 2 ))\n");
				goto PRINT_EXIT;
			}
			GetBest3Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);//********
			if (best_sol.size() == 3) {
				cur_sol.push_back(best_sol[0]);
				cur_sol.push_back(best_sol[1]);
				cur_sol.push_back(best_sol[2]);
		//		printf("(( 3 ))\n");
				goto PRINT_EXIT;
			}
			/*setcover::GetBest4Solution(pvalues_col_copy, rowmask, csum_sortindex, best_sol, ex_sols);
			if (best_sol.size() == 4) {
			cur_sol.push_back(best_sol[0]);
			cur_sol.push_back(best_sol[1]);
			cur_sol.push_back(best_sol[2]);
			cur_sol.push_back(best_sol[3]);
			printf("(( 4 ))\n");
			goto PRINT_EXIT;
			}*/
			if (max_pos_list.size() > 0) {
				cur_sol.push_back(max_pos_list[0]);
		//		printf("SIG5(0) %i\n", max_pos_list[0]);
				rowmask.or_count(pvalues_col_copy[max_pos_list[0]]);
				RemoveRowCol1(pvalues_col_copy, pvalues_row_copy, rowmask, max_pos_list[0]);
				AdjustExSolutions(ex_sols, max_pos_list[0]);
			}
			else {
				// restart to find another solution
				//cur_sol.pop_back();
				//if (cur_sol.size() > 0)
				//  goto START;
				return 0;
			}
		}
	}

/*printf("cur sol: ");
	for (int m = 0; m < cur_sol.size(); m++)
		printf("%i ", cur_sol[m]);
	printf("\n");  */

PRINT_EXIT:
	//printf("time-iter = %f seconds\n", ((float)(clock() - t2)) / CLOCKS_PER_SEC);

    //if (cur_sol.size() > 3)
  if (cur_sol.size() >= 2 && cur_sol.size() <= 5) {
    SortWei(cur_sol);
    ReduceColCompress_All(pvalues_col, cur_sol, inexcluded);
  }
  else if (cur_sol.size() >= 6) {
    SortWei(cur_sol);
    ReduceColCompress(pvalues_col, cur_sol, inexcluded);
  }
  if (cur_sol.size() > 5) {
    SortWei(cur_sol);
    ReduceColReplace31(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);
  }

  //if (cur_sol.size() > 3)
  if (cur_sol.size() >= 2 && cur_sol.size() <= 5) {
    SortWei(cur_sol);
    ReduceColReplace21_Traverse(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);
  }
  else if (cur_sol.size() >= 6) {
    SortWei(cur_sol);
    ReduceColReplace21(pvalues_col, original_csum_sortindex, cur_sol, inexcluded);
  }

//	printf("time-set covering without weight= %f seconds\n", ((float)(clock() - t1)) / CLOCKS_PER_SEC);

	cout << "------Solution without considering weight:--------" << endl;
	printResult(cur_sol);

	//	setcover::solutionVerify(cur_sol); //**************delete at last***************

	for (vector <int>::iterator iter = cur_sol.begin(); iter != cur_sol.end(); iter++)
		selected_cands.push_back(signals[*iter]);

	cout << "--------------Setcovering Over(No weight)---------------" << endl;

	bitvector nomask(pvalues_col[0].size());
	if (!IsSolutionLite(pvalues_col, nomask, cur_sol)) {
		cout << "\n\n\n\n\n\n**********************(SC)Final solution Cannot cover all rows!!!!***************************\n\n\n\n\n\n" << endl;
		return 0;
	}

	return 1;
}

int setcover::OptimizeWeight_ExRange(nodecircuit::NodeVector &current_cands, nodecircuit::NodeVector &new_cands, int ex_col_start, int ex_col_end, const int t_limit)
{
  new_cands = current_cands; // AMIR
	cout << "--------------------Weighted Setcovering Start-------------------" << endl;
	cout << "colsize=" << col_size << "  rowsize=" << row_size << "\n" << endl;
	//clock_t t_wei_start = clock();
	if (!current_cands.size()) {
		cout << "********No candidate here!!**********" << endl;
		return 0;
	}

	int i, j, k;
	vector <int> cur_sol;
	for (int col = 0;col < col_size;col++) {
		if (cur_sol.size() < current_cands.size() && signals[col]->weight) {
			nodecircuit::NodeVector::iterator iter = find(current_cands.begin(), current_cands.end(), signals[col]);
			if (iter != current_cands.end()) {
				cur_sol.push_back(col);
			}
		}
		if (cur_sol.size() == current_cands.size())
			break;
	}

	bitvector nomask(pvalues_col[0].size());
	if (!IsSolutionLite(pvalues_col, nomask, cur_sol)) {
		cout << "\n\n\n\n\n\n**********************Input solution Cannot cover all rows!!!!***************************\n\n\n\n\n\n" << endl;
		return 0;
	}

	vector< bitvector > pvalues_row_copy, pvalues_col_copy;
	pvalues_row_copy.resize(row_size);
	for (i = 0; i < row_size; i++) {
		pvalues_row_copy[i].copy(pvalues_row[i]);
	}
	pvalues_col_copy.resize(col_size);
	/*for (j = 0; j < col_size; j++) {
		pvalues_col_copy[j].copy(pvalues_col[j]);
	}*/

	vector <In_Ex_Type> inexcluded(col_size, No_In_Ex);
	int ex_ret = InExcludeSigs_Range(pvalues_row_copy, pvalues_col_copy, cur_sol, ex_col_start, ex_col_end, inexcluded, 0);
  if (!ex_ret)
    return 0;

	vector<int> original_csum_sortindex;
	//SortCol(pvalues_col, original_csum_sortindex);

	//----------------weight--------------------
	ReduceWeightReplace11_Traverse(inexcluded, pvalues_col, cur_sol);
	//***********set time limit*************** 1s < limit < 600s
	int t_limit_in = 0;
	if (t_limit > 600)
		t_limit_in = 600;
	else if (t_limit < 1)
		t_limit_in = 1;
	else
		t_limit_in = t_limit;
	cout << "\n----------Time limit for WeightRe22 is: " << t_limit_in << " seconds----------" << endl;
	// AMIR
  //if (cur_sol.size() >= 2)
	//	ReduceWeightReplace22(pvalues_col, original_csum_sortindex, cur_sol, inexcluded, t_limit_in);
	//------------------------------------------

  //AMIR
  int original_weight = 0;
  for (int cnt = 0; cnt < current_cands.size(); cnt++)
    original_weight += current_cands[cnt]->weight;

  int new_weight = 0;
  new_cands.clear();
  for (vector <int>::iterator iter = cur_sol.begin(); iter != cur_sol.end(); iter++) {
    new_cands.push_back(signals[*iter]);
    new_weight += new_cands.back()->weight;
  }

  //AMIR . revert back to original if not good!
  if (new_weight > original_weight || (new_weight == original_weight && new_cands.size() >= current_cands.size()))
    new_cands = current_cands;

	cout << "\n--Final solution after optimization of weight--" << endl;
	printResult(cur_sol);
	//	solutionVerify(cur_sol);
	//printf("time-set reduced weight= %f seconds\n", ((float)(clock() - t_wei_start)) / CLOCKS_PER_SEC);
	cout << "\n\n-----------------------------Weighted Setcovering Over-----------------------------\n\n\n\n" << endl;

	if (!IsSolutionLite(pvalues_col, nomask, cur_sol)) {
		cout << "\n\n\n\n\n\n**********************(WSC)Final solution Cannot cover all rows!!!!***************************\n\n\n\n\n\n" << endl;
		new_cands = current_cands;
		return 0;
	}

	return 1;
}

  int setcover::ReduceColCompress_All(std::vector< bitvector > &pvalues_col, std::vector<int> &sol) {
    vector < vector<int> > all_sol;
    int pre_sol_num = 0;
    all_sol.push_back(sol);
    bitvector nomask(pvalues_col[0].size());

    while (pre_sol_num < all_sol.size())
    {
      //delete previous sol
      int i = 0;
      vector < vector<int> >::iterator iter = all_sol.begin();
      while (i < pre_sol_num)
      {
        iter = all_sol.erase(iter);
        i++;
      }
      pre_sol_num = all_sol.size();
      for (int i = 0;i < pre_sol_num;i++)
      {
        for (vector<int>::iterator it = all_sol[i].begin(); it != all_sol[i].end(); it++)
        {
          {  //--New: The included signals will not be deleted
            vector<int> tempsol;
            for (int j = 0; j < all_sol[i].size(); j++)
              if (all_sol[i][j] != *it)
                tempsol.push_back(all_sol[i][j]);
            if (find(all_sol.begin(), all_sol.end(), tempsol) != all_sol.end())
              continue;
          }

        }
      }
    }


    if (all_sol[0].size() < sol.size())
    {
      int best_sol_size = all_sol[0].size();
      int best_sol_index = 0;
			// cout << "new sol(Compress_All)" << endl;
			// int best_wei = printResult(all_sol[0]);
			 int best_wei = 0;
			 for (vector<int>::iterator iter = all_sol[0].begin();iter != all_sol[0].end();iter++)
					 best_wei = best_wei + signals[*iter]->weight;

      for (int i = 1;i < all_sol.size();i++)
      {
				// cout << "new sol(Compress_All)" << endl;
			 //  int cur_wei = printResult(all_sol[i]);
				 int cur_wei = 0;
				 for (vector<int>::iterator iter = all_sol[i].begin();iter != all_sol[i].end();iter++)
						 cur_wei = cur_wei + signals[*iter]->weight;
        if (all_sol[i].size() < best_sol_size || (all_sol[i].size() == best_sol_size && cur_wei < best_wei))
        {
          best_wei = cur_wei;
          best_sol_index = i;
          best_sol_size = all_sol[i].size();
        }

      }
      sol.clear();
      sol.assign(all_sol[best_sol_index].begin(), all_sol[best_sol_index].end());
      cout << "Final new sol(Compress_All)" << endl;
      printResult(sol);
    }


    return sol.size();
  }

  int setcover::ReduceColCompress(std::vector< bitvector > &pvalues_col, std::vector<int> &sol) {
    bitvector nomask(pvalues_col[0].size());
    for (vector<int>::iterator it = sol.begin(); it != sol.end(); ) {
       {
        vector<int> tempsol;
        for (int j = 0; j < sol.size(); j++) {
          if (sol[j] != *it)
            tempsol.push_back(sol[j]);
        }
        if (IsSolutionLite(pvalues_col, nomask, tempsol)) {
          it = sol.erase(it);
        /*  printf("new sol(Co): ");
          for (int m = 0; m < sol.size(); m++)
            printf("%i ", sol[m]);
          printf("\n"); */
        }
        else
          ++it;
      }
    }

    return sol.size();
  }

  int setcover::ReduceColReplace21(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol) {
    for (int i = 0; i < sol.size() - 1; i++) {
      for (int j = i + 1; j < sol.size(); j++) {
         {
          bitvector both;
          both.copy(pvalues_col[sol[i]]);
          int cnt = both.or_count(pvalues_col[sol[j]]);		//both = col_i | col_j
          for (int m = 0; m < sol.size(); m++) {
            if (m != i && m != j)
              cnt = both.mask_count(pvalues_col[sol[m]]); //!!!both = the rows only covered by col_i and col_j!!!
          }
          int k = 0;  //k: the number of cols that covered more rows than "both". Only check k columns below
          while (k < csum_sortindex.size() && pvalues_col[csum_sortindex[k]].count() >= cnt)
            k++;
          k--;
          while (k >= 0) {
            int newcol = csum_sortindex[k];
            if (find(sol.begin(), sol.end(), newcol) != sol.end()) {	//make sure newcol isn't inside current sol
              k--;
              continue;
            }
            if (cnt <= pvalues_col[newcol].count()) { //don't replace with excluded signal
              int cnt2 = count_and(both, pvalues_col[newcol]);
              if (cnt2 == cnt) {
                sol[j] = sol[sol.size() - 1]; //shift the last elements to redundant signal's place, then pop
                sol.pop_back();
                sol[i] = sol[sol.size() - 1];
                sol.pop_back();
                sol.push_back(newcol);
                SortWei(sol);
                j = sol.size() + 2;
                i = -1;					   //try to replace from beginning of sol
              /*  printf("new sol(Re21): ");
                for (int m = 0; m < sol.size(); m++)
                  printf("%i ", sol[m]);
                printf("\n");  */
                break;
              }
              else
                k--;
            }
          }
        }
      }
    }
    return sol.size();
  }

  int setcover::ReduceColReplace21_Traverse(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol) {
    int best_sol_size = sol.size();
    vector <int> best_sol;

    for (int start_sig = 0; start_sig < sol.size();start_sig++)
    {
      vector<int>tmp_sol(sol.size());

      for (int i = start_sig;i < sol.size();i++)
        tmp_sol[i - start_sig] = sol[i];
      for (int i = 0;i < start_sig;i++)
        tmp_sol[sol.size() - start_sig + i] = sol[i];

      ReduceColReplace21(pvalues_col, csum_sortindex, tmp_sol);

      if (best_sol_size > tmp_sol.size())
      {
        best_sol_size = tmp_sol.size();
        best_sol.clear();
        best_sol.assign(tmp_sol.begin(), tmp_sol.end());
      }
    }

    if (best_sol_size < sol.size())
    {
      sol.clear();
      sol.assign(best_sol.begin(), best_sol.end());
      cout << "------Traverse final new sol(Re21)-------" << endl;
      printResult(sol);
    }

  }

  int setcover::ReduceColReplace31(std::vector< bitvector > &pvalues_col, std::vector<int> &csum_sortindex, std::vector<int> &sol) {
    for (int i = 0; i < sol.size() - 2; i++) {
      for (int j1 = i + 1; j1 < sol.size() - 1; j1++) {
        for (int j2 = j1 + 1; j2 < sol.size(); j2++) {
          {
            bitvector both;
            both.copy(pvalues_col[sol[i]]);
            both.or_count(pvalues_col[sol[j1]]);
            int cnt = both.or_count(pvalues_col[sol[j2]]);
            for (int m = 0; m < sol.size(); m++) {
              if (m != i && m != j1 && m != j2)
                cnt = both.mask_count(pvalues_col[sol[m]]);
            }
            int k = 0;
            while (k < csum_sortindex.size() && pvalues_col[csum_sortindex[k]].count() >= cnt)
              k++;
            k--;
            while (k >= 0 ) {
              int newcol = csum_sortindex[k];
              if (find(sol.begin(), sol.end(), newcol) != sol.end()) {	//make sure newcol isn't inside current sol
                k--;
                continue;
              }
              if (cnt <= pvalues_col[newcol].count()) { //don't replace with excluded signal
                int cnt2 = count_and(both, pvalues_col[newcol]);
                if (cnt2 == cnt) {
                  sol[j2] = sol[sol.size() - 1];
                  sol.pop_back();
                  sol[j1] = sol[sol.size() - 1];
                  sol.pop_back();
                  sol[i] = sol[sol.size() - 1];
                  sol.pop_back();
                  sol.push_back(newcol);
                  SortWei(sol);
                  j1 = sol.size() + 2;
                  j2 = sol.size() + 2;
                  i = -1;
              /*    printf("new sol(Re31): ");
                  for (int m = 0; m < sol.size(); m++)
                    printf("%i ", sol[m]);
                  printf("\n"); */
                  break;
                }
                else
                  k--;
              }
            }
          }
        }
      }
    }

//    printf("\n");
    return sol.size();
  }

}

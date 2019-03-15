//
// Created by iwata on 17/06/22.
//

#include <fstream>
#include <regex>
//#include <assert.h>
#include <iostream>
#include "CNFGenerator.h"

using namespace CNFgen;

using Glucose::mkLit;
using psi = std::pair<std::string, int>;
using ABC::Abc_FrameReplaceCurrentNetwork;
using ABC::Abc_Ntk_t;
using ABC::Io_ReadCreatePi; using ABC::Io_ReadCreatePo;
using ABC::Abc_NtkFinalizeRead;
using ABC::Abc_Obj_t;
using ABC::Io_ReadCreateNode;
using ABC::Abc_ObjSetData;
using ABC::Mem_Flex_t;
using ABC::Abc_ObjName;
using ABC::Abc_ObjFanin0;
using ABC::Abc_ObjFanout0;
using psi = std::pair<std::string, int>;

// c: circuit to be converted into CNF
// nodes: nodes which can be included in [lit-number:name] map
//
// The mapping-data for PI&PO is made by default. If you want to observe other nodes, you can add as a argument.

CNFGenerator::CNFGenerator() {

}

int CNFGenerator::Init(std::string name, nodecircuit::Circuit *c, nodecircuit::NodeVector *ex_in, nodecircuit::NodeVector *ex_out) {
	circuit_org = c;

	ABC::Abc_Ntk_t *pNtk, *pTemp;
	ABC::Abc_Frame_t *pAbc;

	pNtk = ABC::Abc_NtkStartRead(strdup(name.c_str()));
	Circuit2Abc(pNtk, c, ex_in, ex_out);

	pAbc = ABC::Abc_FrameGetGlobalFrame();
	Abc_FrameReplaceCurrentNetwork(pAbc, pNtk);
	ABC::Abc_NtkFinalizeRead(pNtk);
	if (ABC::Abc_NtkCheckRead(pNtk) == 0) {
		std::cout << "Circuit is wrong" << std::endl;
		//assert((0);
	}
	pNtk = ABC::Abc_NtkToLogic(pTemp = pNtk);

	ABC::Abc_FrameReplaceCurrentNetwork(pAbc, pNtk); ABC::Abc_FrameClearVerifStatus(pAbc);
	char Command[300] = "balance -l; rewrite -l; refactor -l; balance -l; rewrite -l; rewrite -z -l; balance -l; refactor -z -l; rewrite -z -l; balance -l;";
	//char Command[100] = "strash;dc2;dc2;";
	if (ABC::Cmd_CommandExecute(pAbc, Command)) {
		std::cout << "Command execution error: " << Command << std::endl;
		exit(1);
	}
	//pTemp = ABC::Abc_NtkToNetlistBench(pAbc->pNtkCur);
	//Abc2CNF(pTemp);

	pTemp = Abc_NtkToNetlist(pAbc->pNtkCur);
	if (!ABC::Abc_NtkHasSop(pTemp) && !ABC::Abc_NtkHasMapping(pTemp))
		ABC::Abc_NtkToSop(pTemp, -1, ABC_INFINITY);
	Abc2CNF2(pTemp);

	Finalize(ex_in, ex_out);
	return 0;
}

CNFGenerator::CNFGenerator(std::string name, nodecircuit::Circuit *c, nodecircuit::NodeVector *ex_in, nodecircuit::NodeVector *ex_out) {
	circuit_org = c;

	ABC::Abc_Ntk_t *pNtk, *pTemp;
	ABC::Abc_Frame_t *pAbc;

	pNtk = ABC::Abc_NtkStartRead(strdup(name.c_str()));
	Circuit2Abc(pNtk, c, ex_in, ex_out);

	pAbc = ABC::Abc_FrameGetGlobalFrame();
	Abc_FrameReplaceCurrentNetwork(pAbc, pNtk);
	ABC::Abc_NtkFinalizeRead(pNtk);
	if (ABC::Abc_NtkCheckRead(pNtk) == 0) {
		std::cout << "Circuit is wrong" << std::endl;
		//assert((0);
	}
	pNtk = ABC::Abc_NtkToLogic(pTemp = pNtk);

	ABC::Abc_FrameReplaceCurrentNetwork(pAbc, pNtk); ABC::Abc_FrameClearVerifStatus(pAbc);
	char Command[300] = "balance -l; rewrite -l; refactor -l; balance -l; rewrite -l; rewrite -z -l; balance -l; refactor -z -l; rewrite -z -l; balance -l;";
	//char Command[100] = "strash;dc2;dc2;";
	if (ABC::Cmd_CommandExecute(pAbc, Command)) {
		std::cout << "Command execution error: " << Command << std::endl;
		exit(1);
	}
	//pTemp = ABC::Abc_NtkToNetlistBench( pAbc->pNtkCur );
	//Abc2CNF(pTemp);

	pTemp = Abc_NtkToNetlist(pAbc->pNtkCur);
	if (!ABC::Abc_NtkHasSop(pTemp) && !ABC::Abc_NtkHasMapping(pTemp))
		ABC::Abc_NtkToSop(pTemp, -1, ABC_INFINITY);
	Abc2CNF2(pTemp);

	Finalize(ex_in, ex_out);
}


int CNFGenerator::Circuit2Abc(Abc_Ntk_t *pNtk, nodecircuit::Circuit *c, nodecircuit::NodeVector *ex_in, nodecircuit::NodeVector *ex_out) {
	//read PI & PO
	for (auto input = begin(c->inputs); input != end(c->inputs); input++) {
		Io_ReadCreatePi(pNtk, strdup(((*input)->name).c_str()));
	}
	for (auto output = begin(c->outputs); output != end(c->outputs); output++) {
		Io_ReadCreatePo(pNtk, strdup(((*output)->name).c_str()));
	}
	//read ex_in & ex_out
	for (auto tmp = (*ex_in).begin(); tmp != (*ex_in).end(); tmp++) {
		if ((*tmp)->is_input)
			continue;
		Io_ReadCreatePi(pNtk, strdup(((*tmp)->name).c_str()));
	}
	for (auto tmp = (*ex_out).begin(); tmp != (*ex_out).end(); tmp++) {
		if ((*tmp)->is_output)
			continue;
		Io_ReadCreatePo(pNtk, strdup(((*tmp)->name).c_str()));
	}

	for (auto node = begin(c->all_nodes); node != end(c->all_nodes); node++) {
		bool ignoreFlag = false;
		for (auto exin : *ex_in) {
			if ((*node)->name == exin->name && (*node)->inputs.size() != 0) {
				ignoreFlag = true;
			}
		}
		for (auto in : c->inputs) {
			if ((*node)->name == in->name && (*node)->inputs.size() != 0) {
				ignoreFlag = true;
			}
		}
		if (ignoreFlag) {
			continue;
		}

		bool FLAG = true;
		if ((*node)->is_input) {
			continue;
		}

		int nNames = (*node)->inputs.size();

		char** pNames = (char**)malloc(sizeof(char*)*nNames);
		for (int i = 0; i < nNames; i++) {
			pNames[i] = strdup((*node)->inputs[i]->name.c_str());
		}
		//for (int i = 0; i < nNames; i++) {
		//	pNames[i] = strdup((*node)->inputs[nNames-1-i]->name.c_str());
		//}

		for (auto tmp = (*ex_in).begin(); FLAG && tmp != (*ex_in).end(); tmp++) {
			FLAG = ((*tmp) != (*node));
		}//repetitive feature?

		if (FLAG) {
			//std::cout <<(*node)->name<<":"<< (*node)->cube << std::endl;
			Abc_Obj_t * pNode = Io_ReadCreateNode(pNtk, strdup((*node)->name.c_str()), pNames, nNames);
			char* pSop = new char[99999];
			strcpy(pSop,(*node)->cube.c_str());
			int flag = 0;
			for (char* pCur = pSop; *pCur; pCur++) {
				if (*pCur == '\n')
					flag = 1;
			}
			if (flag == 0) {
				std::cout <<(*node)->name<<":"<< (*node)->cube << std::endl;
			}
			assert(flag == 1);
			ABC::Abc_ObjSetData(pNode, ABC::Abc_SopRegister((ABC::Mem_Flex_t *) pNtk->pManFunc, (*node)->cube.c_str()));
			
		}
	}
	return 0;
}

int CNFgen::CNFGenerator::Abc2CNF(ABC::Abc_Ntk_t *pNtk) {

	//assert((ABC::Abc_NtkIsSopNetlist(pNtk));
	int index = 0;

	Abc_Obj_t * pNode; int i;
	Abc_NtkForEachPi(pNtk, pNode, i) {
		//signals.insert(std::string(Abc_ObjName(Abc_ObjFanout0(pNode))));
		if (signals.insert(std::pair<std::string, int>{std::string(Abc_ObjName(Abc_ObjFanout0(pNode))), index}).second)
			index++;
	}
	Abc_NtkForEachPo(pNtk, pNode, i) {
		//signals.insert(std::string(Abc_ObjName(Abc_ObjFanin0(pNode))));
		if (signals.insert(std::pair<std::string, int>{std::string(Abc_ObjName(Abc_ObjFanin0(pNode))), index}).second)
			index++;
	}
	Abc_NtkForEachLatch(pNtk, pNode, i) {
		std::cout << "Latch isn't supported" << std::endl;
		//assert((0);
	}
	Abc_NtkForEachNode(pNtk, pNode, i) {
		//signals.insert(std::string(Abc_ObjName(Abc_ObjFanout0(pNode))));
		if (signals.insert(std::pair<std::string, int>{std::string(Abc_ObjName(Abc_ObjFanout0(pNode))), index}).second)
			index++;
		int nFanins = ABC::Abc_ObjFaninNum(pNode);
		if (nFanins >= 1) {
			//signals.insert(std::string(Abc_ObjName(Abc_ObjFanin0(pNode))));
			if (signals.insert(std::pair<std::string, int>{std::string(Abc_ObjName(Abc_ObjFanin0(pNode))), index}).second)
				index++;
			if (nFanins >= 2)
				//signals.insert(std::string(Abc_ObjName(Abc_ObjFanin1(pNode))));
				if (signals.insert(std::pair<std::string, int>{std::string(Abc_ObjName(Abc_ObjFanin1(pNode))), index}).second)
					index++;
		}
	}


	Abc_NtkForEachNode(pNtk, pNode, i) {
		//assert((ABC::Abc_ObjIsNode(pNode));
		int nFanins = ABC::Abc_ObjFaninNum(pNode);

		int var_out = GetIndex(std::string(Abc_ObjName(Abc_ObjFanout0(pNode))));
		std::map<int, bool> lits;
		if (nFanins == 0) {   // write the constant 1 node
		  //assert((Abc_NodeIsConst1(pNode));
			lits[var_out] = true;
			clauses.push_back(lits);
		}
		else if (nFanins == 1) {   // write the interver/buffer
			int var_in1 = GetIndex(std::string(Abc_ObjName(Abc_ObjFanin0(pNode))));
			if (Abc_NodeIsBuf(pNode)) {
				//BUFF;
				lits[var_out] = true;
				lits[var_in1] = false;
				clauses.push_back(lits);
				lits.clear();
				//lits = new Glucose::vec<Glucose::Lit>;
				lits[var_out] = false;
				lits[var_in1] = true;
				clauses.push_back(lits);
			}
			else {
				//NOT;
				lits[var_out] = false;
				lits[var_in1] = false;
				clauses.push_back(lits);
				lits.clear();
				//lits = new Glucose::vec<Glucose::Lit>;
				lits[var_out] = true;
				lits[var_in1] = true;
				clauses.push_back(lits);
			}
		}
		else {   // write the AND gate
			int var_in1 = GetIndex(std::string(Abc_ObjName(Abc_ObjFanin0(pNode))));
			int var_in2 = GetIndex(std::string(Abc_ObjName(Abc_ObjFanin1(pNode))));
			lits[var_out] = true; lits[var_in1] = false; lits[var_in2] = false;
			clauses.push_back(lits);
			//lits = new Glucose::vec<Glucose::Lit>;
			lits.clear();
			lits[var_out] = false; lits[var_in1] = true;
			clauses.push_back(lits);
			//lits = new Glucose::vec<Glucose::Lit>;
			lits.clear();
			lits[var_out] = false; lits[var_in2] = true;
			clauses.push_back(lits);
		}
	}
	return 0;
}

void CNFGenerator::dumpcnf() {
	for (auto itr : signals)
		std::cout << itr.first << ": " << itr.second << std::endl;

	for (auto itr : clauses) {
		for (auto lit : itr)
			std::cout << ((lit.second == true) ? "" : "-") << lit.first << ",";
		std::cout << std::endl;
	}
	return;
}

void CNFGenerator::writeDimacs(std::string filename) {
	std::ofstream dimacs;
	dimacs.open(filename, std::ios::out);

	dimacs << "p cnf " << signals.size() << " " << clauses.size() << std::endl;

	for (auto sig : signals)
		dimacs << "c " << sig.first << " " << sig.second + 1 << std::endl;

	for (auto itr : clauses) {
		for (auto lit : itr)
			dimacs << ((lit.second == true) ? "" : "-") << lit.first + 1 << " ";
		dimacs << "0" << std::endl;
	}
	dimacs.close();
}

void CNFGenerator::Dump()
{
	for (auto sig : signals)
		std::cout << sig.first << ":" << sig.second << ",";
	std::cout << std::endl;
	for (auto cl : clauses) {
		for (auto lit : cl) {
			std::cout << (lit.second == false ? "-" : "") << lit.first << " ";
		}
		std::cout << std::endl;
	}
}

int CNFGenerator::GetIndex(std::string nd) {
	//std::cout << "GetIndex: ";
	auto itr = cache_string.find(nd);
	if (itr != cache_string.end()) {
		//std::cout << (*itr).second << std::endl;
		return (*itr).second;
	}

	auto itr3 = signals.find(nd);
	if (itr3 != signals.end()) {
		//std::cout << (*itr3).second << std::endl;
		return (*itr3).second;
	}
	else {
		std::cout << "no index" << std::endl;
		return -1;
	}
}

int CNFGenerator::GetIndex(nodecircuit::Node * nd) {
	//std::cout << "GetIndex";
	auto itr = cache_Node.find(nd);
	if (itr != cache_Node.end()) {
		//std::cout << (*itr).second << std::endl;
		return (*itr).second;
	}

	auto itr2 = cache_string.find(nd->name);
	if (itr2 != cache_string.end()) {
		//std::cout << (*itr2).second << std::endl;
		return (*itr2).second;
	}

	auto itr3 = signals.find(nd->name);
	if (itr3 != signals.end()) {
		//std::cout << (*itr3).second << std::endl;
		return (*itr3).second;
	}
	else {
		std::cout << "no index" << std::endl;
		return -1;
	}
}

void CNFGenerator::Finalize(nodecircuit::NodeVector *ex_in, nodecircuit::NodeVector *ex_out) {
	for (auto nd : circuit_org->inputs) {
		cache_Node[nd] = signals[nd->name];
		cache_string[nd->name] = signals[nd->name];
	}
	for (auto nd : circuit_org->outputs) {
		cache_Node[nd] = signals[nd->name];
		cache_string[nd->name] = signals[nd->name];
	}
	for (auto nd : *ex_in) {
		cache_Node[nd] = signals[nd->name];
		cache_string[nd->name] = signals[nd->name];
	}
	for (auto nd : *ex_out) {
		cache_Node[nd] = signals[nd->name];
		cache_string[nd->name] = signals[nd->name];
	}
}

int CNFgen::CNFGenerator::Abc2CNF2(ABC::Abc_Ntk_t *pNtk) {

	//assert((Abc_NtkIsNetlist(pNtk));
	int index = 0;

	Abc_Obj_t *pNode, *pNet; int i, j;
	Abc_NtkForEachPi(pNtk, pNode, i) {
		//signals.insert(std::string(Abc_ObjName(Abc_ObjFanout0(pNode))));
		if (signals.insert(std::pair<std::string, int>{std::string(Abc_ObjName(Abc_ObjFanout0(pNode))), index}).second)
			index++;
	}
	Abc_NtkForEachPo(pNtk, pNode, i) {
		//signals.insert(std::string(Abc_ObjName(Abc_ObjFanin0(pNode))));
		if (signals.insert(std::pair<std::string, int>{std::string(Abc_ObjName(Abc_ObjFanin0(pNode))), index}).second)
			index++;
	}
	Abc_NtkForEachLatch(pNtk, pNode, i) {
		std::cout << "Latch isn't supported" << std::endl;
		//assert((0);
	}

	Abc_NtkForEachNode(pNtk, pNode, i) {
		char* pName;
		int nFanins = ABC::Abc_ObjFaninNum(pNode);;
		int iOut;
		std::vector<int> iIn;

		Abc_ObjForEachFanin(pNode, pNet, j) {
			pName = Abc_ObjName(pNet);
			//signals.insert(std::string(pName));
			if (signals.insert(std::pair<std::string, int>{std::string(pName), index}).second) {
				iIn.push_back(index++);
			}
			else {
				iIn.push_back(signals[std::string(pName)]);
			}
		}
		pName = Abc_ObjName(Abc_ObjFanout0(pNode));
		//signals.insert(std::string(pName));
		if (signals.insert(std::pair<std::string, int>{std::string(pName), index}).second) {
			iOut = index++;
		}
		else {
			iOut = signals[std::string(pName)];
		}

		std::map<int, bool> lits;
		if (nFanins == 0) {
			if (Abc_NodeIsConst1(pNode)) {
				lits[iOut] = true;
			}
			else {
				lits[iOut] = false;
			}
			clauses.push_back(lits); lits.clear();
		}
		else if (nFanins == 1) {
			if (Abc_NodeIsBuf(pNode)) {
				//BUFF;
				lits[iOut] = true;
				lits[iIn[0]] = false;
				clauses.push_back(lits); lits.clear();
				lits[iOut] = false;
				lits[iIn[0]] = true;
				clauses.push_back(lits); lits.clear();
			}
			else {
				//NOT;
				lits[iOut] = false;
				lits[iIn[0]] = false;
				clauses.push_back(lits); lits.clear();
				lits[iOut] = true;
				lits[iIn[0]] = true;
				clauses.push_back(lits); lits.clear();
			}
		}
		else if (nFanins == 2) {
			std::vector<bool> logic;
			std::string buffer = (char*)Abc_ObjData(pNode);
			//std::cout << "DEBUG: " << buffer;

			logic.push_back(buffer[0] - '0');
			logic.push_back(buffer[1] - '0');
			logic.push_back(buffer[3] - '0');
			//assert((buffer[0] == '0' || buffer[0] == '1');
			//assert((buffer[1] == '0' || buffer[1] == '1');
			//assert((buffer[3] == '0' || buffer[3] == '1');

			//std::cout << logic[0] << logic[1] << std::endl;

			lits[iIn[0]] = ((logic[0] == true) ? false : true); lits[iIn[1]] = ((logic[1] == true) ? false : true); lits[iOut] = ((logic[2] == true) ? true : false);
			clauses.push_back(lits); lits.clear();
			lits[iIn[0]] = ((logic[0] == true) ? true : false); lits[iOut] = ((logic[2] == true) ? false : true);
			clauses.push_back(lits); lits.clear();
			lits[iIn[1]] = ((logic[1] == true) ? true : false); lits[iOut] = ((logic[2] == true) ? false : true);
			clauses.push_back(lits); lits.clear();
		}
		else {
			std::cout << "Not supported circuit" << std::endl;
			//assert((0);
		}

	}

	// Suppose the number of inputs is 0,1,2
	/*
	Abc_NtkForEachNode(pNtk, pNode, i) {
	  int nFanins = ABC::Abc_ObjFaninNum(pNode);
	  int iOut = GetIndex(Abc_ObjName(Abc_ObjFanout0(pNode)));
	  std::vector<int> iIn;// = { GetIndex(Abc_ObjName(Abc_ObjFanin(pNode, 0))), GetIndex(Abc_ObjName(Abc_ObjFanin(pNode, 1))) };
	  for (int j = 0; j < nFanins; j++)
		iIn.push_back(GetIndex(Abc_ObjName(Abc_ObjFanin(pNode, j))));

	  std::map<int, bool> lits;
	  if (nFanins == 0) {
		// Suppose constant 1 !
		lits[iOut] = true;
		clauses.push_back(lits); lits.clear();
	  }
	  else if (nFanins == 1) {
		if (Abc_NodeIsBuf(pNode)) {
		  //BUFF;
		  lits[iOut] = true;
		  lits[iIn[0]] = false;
		  clauses.push_back(lits); lits.clear();
		  lits[iOut] = false;
		  lits[iIn[0]] = true;
		  clauses.push_back(lits); lits.clear();
		}
		else {
		  //NOT;
		  lits[iOut] = false;
		  lits[iIn[0]] = false;
		  clauses.push_back(lits); lits.clear();
		  lits[iOut] = true;
		  lits[iIn[0]] = true;
		  clauses.push_back(lits); lits.clear();
		}
	  }
	  else if (nFanins == 2) {
		std::vector<bool> logic;
		std::string buffer = (char*)Abc_ObjData(pNode);
		//std::cout << "DEBUG: " << buffer;

		logic.push_back(buffer[0]-'0');
		logic.push_back(buffer[1]-'0');
		logic.push_back(buffer[3] - '0');
		//assert((buffer[0] == '0' || buffer[0] == '1');
		//assert((buffer[1] == '0' || buffer[1] == '1');
		//assert((buffer[3] == '0' || buffer[3] == '1');

		//std::cout << logic[0] << logic[1] << std::endl;

		lits[iIn[0]] = ((logic[0] == true) ? false : true); lits[iIn[1]] = ((logic[1] == true) ? false : true); lits[iOut] = ((logic[2] == true) ? true : false);
		clauses.push_back(lits); lits.clear();
		lits[iIn[0]] = ((logic[0] == true) ? true : false); lits[iOut] = ((logic[2] == true) ? false : true);
		clauses.push_back(lits); lits.clear();
		lits[iIn[1]] = ((logic[1] == true) ? true : false); lits[iOut] = ((logic[2] == true) ? false : true);
		clauses.push_back(lits); lits.clear();
	  }
	  else {
		std::cout << "Not supported circuit" << std::endl;
		//assert((0);
	  }
	}
	*/
	return 0;
}
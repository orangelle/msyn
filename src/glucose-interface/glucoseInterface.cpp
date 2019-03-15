#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <assert.h>
#include <chrono>

#include "utils/System.h"
#include "core/Dimacs.h"
#include "simp/SimpSolver.h"
#include "glucoseInterface.h"

using namespace Glucose;

GInterface::GInterface() {
  solver = new SimpSolver();
  initSolver();
}

GInterface::GInterface(const GInterface &old) {
  solver = new SimpSolver(*old.solver);
  //initSolver();
  dimacsFile = old.dimacsFile;
  //initSolver();
}

void GInterface::initSolver() {
#if defined(__linux__)
  fpu_control_t oldcw, newcw;
  _FPU_GETCW(oldcw);
  newcw = (oldcw & ~_FPU_EXTENDED) | _FPU_DOUBLE;
  _FPU_SETCW(newcw);
#endif
  solver->eliminate(false);
  solver->setIncrementalMode();
  solver->parsing = 1;
  solver->verbosity = 0;
  return;
}

Lit GInterface::genLit(int x, bool b) {
  //std::cout << "Literal " << (b?"":"-") << x << std::endl;
  if (x < 0) {
    printf("Lit: %d\n", x);
    //assert(x >= 0);
  }
  x = abs(x) /*- 1*/;
  while (x >= solver->nVars())
    solver->newVar();
  if (b) {
    return mkLit(x);
  }
  else {
    return ~mkLit(x);
  }
}

void GInterface::addCNF(CNFgen::CNFGenerator *cnf, int offset) {
  for (auto cl = begin(cnf->clauses); cl != end(cnf->clauses); cl++) {
    Glucose::vec<Glucose::Lit> tmp;
    for (auto lit : (*cl)) {
      tmp.push(genLit(lit.first + offset, lit.second));
      //std::cout << ((lit.second == true) ? "" : "-") << lit.first + offset << ",";
    }
    //std::cout << std::endl;
    //std::cout << "clause debuggin... size="<< tmp.size() << "\n";
    //for (int i = 0; i < tmp.size(); i++)
    //  std::cout << tmp[i].x << ",";
    //std::cout << std::endl;

    //std::cout << "pushing... ";
    solver->addClause_(tmp);
    //std::cout << "pushed\n";
  }
  return;
}

void GInterface::addDimacs(const char* dimacsFile, int offset) {
  solver->parsing = 1;
  if (dimacsFile == NULL) {
    std::cout << "No input dimacs\n";
    exit(1);
  }
  gzFile fp = gzopen(dimacsFile, "rb");
  StreamBuffer in(fp);
  for (;;) {
    skipWhitespace(in);
    if (*in == EOF)
      break;
    else if (*in == 'c' || *in == 'p')
      skipLine(in);
    else {
      vec<Lit> lits;
      readClause2(in, lits, offset);
      solver->addClause_(lits);
    }
  }
  gzclose(fp);
  return;
}

void GInterface::setNEqual(std::vector<int> values1, std::vector<int> values2) {
  //assert(values1.size() == values2.size());

  int max = solver->nVars();

  for (int i = 0; i < values1.size(); i++) {
    vec<Lit> lits1, lits2, lits3, lits4;
    lits1.push(genLit(values1[i], false));
    lits1.push(genLit(values2[i], false));
    lits1.push(genLit(max + i, false));
    lits2.push(genLit(values1[i], false));
    lits2.push(genLit(values2[i], true));
    lits2.push(genLit(max + i, true));
    lits3.push(genLit(values1[i], true));
    lits3.push(genLit(values2[i], true));
    lits3.push(genLit(max + i, false));
    lits4.push(genLit(values1[i], true));
    lits4.push(genLit(values2[i], false));
    lits4.push(genLit(max + i, true));
    solver->addClause_(lits1);
    solver->addClause_(lits2);
    solver->addClause_(lits3);
    solver->addClause_(lits4);
  }

  vec<Lit> tmp;
  for (int i = 0; i < values1.size(); i++)
    tmp.push(genLit(max + i, true));
  solver->addClause_(tmp);
  return;
}

void Glucose::GInterface::ifTrue_thenEQ_elseNEQ(std::vector<int> values1, std::vector<int> values2, int flag)
{
  ifTrue_thenEQ(values1, values2, flag);
  ifFalse_thenNEQ(values1, values2, flag);
}

void Glucose::GInterface::ifTrue_thenNEQ_elseEQ(std::vector<int> values1, std::vector<int> values2, int flag)
{
  ifTrue_thenNEQ(values1, values2, flag);
  ifFalse_thenEQ(values1, values2, flag);
}

void GInterface::setEqual(std::vector<int> values1, std::vector<int> values2) {
  //assert(values1.size() == values2.size());
  for (int i=0; i < values1.size(); i++){
    setEqual(values1[i], values2[i]);
  }
  return;
}

void GInterface::setEqual(int a, int b) {
  //std::cout << "DEBUG setEqual: " << a << "," << b << std::endl;
  if (a == b)
    return;
  vec<Lit> lits1, lits2;
  lits1.push(genLit(a, true));
  lits1.push(genLit(b, false));
  lits2.push(genLit(b, true));
  lits2.push(genLit(a, false));
  solver->addClause_(lits1);
  solver->addClause_(lits2);
  return;
}


void GInterface::ifTrue_thenNEQ(std::vector<int> values1, std::vector<int> values2, int flag) {
  //assert(values1.size() == values2.size());
  
  genLit(flag, true);
  int max = solver->nVars();

  for (int i = 0; i < values1.size(); i++) {
    vec<Lit> lits1, lits2, lits3, lits4;
    lits1.push(genLit(values1[i], false));
    lits1.push(genLit(values2[i], false));
    lits1.push(genLit(max + i, false));
    lits2.push(genLit(values1[i], false));
    lits2.push(genLit(values2[i], true));
    lits2.push(genLit(max + i, true));
    lits3.push(genLit(values1[i], true));
    lits3.push(genLit(values2[i], true));
    lits3.push(genLit(max + i, false));
    lits4.push(genLit(values1[i], true));
    lits4.push(genLit(values2[i], false));
    lits4.push(genLit(max + i, true));
    lits1.push(genLit(flag, false)); lits2.push(genLit(flag, false)); lits3.push(genLit(flag, false)); lits4.push(genLit(flag, false));
    solver->addClause_(lits1);
    solver->addClause_(lits2);
    solver->addClause_(lits3);
    solver->addClause_(lits4);
  }

  vec<Lit> tmp;
  for (int i = 0; i < values1.size(); i++)
    tmp.push(genLit(max + i, true));
  tmp.push(genLit(flag, false));
  solver->addClause_(tmp);
  return;
}

void GInterface::ifFalse_thenNEQ(std::vector<int> values1, std::vector<int> values2, int flag) {
  //assert(values1.size() == values2.size());

  genLit(flag, true);
  int max = solver->nVars();

  for (int i = 0; i < values1.size(); i++) {
    vec<Lit> lits1, lits2, lits3, lits4;
    lits1.push(genLit(values1[i], false));
    lits1.push(genLit(values2[i], false));
    lits1.push(genLit(max + i, false));
    lits2.push(genLit(values1[i], false));
    lits2.push(genLit(values2[i], true));
    lits2.push(genLit(max + i, true));
    lits3.push(genLit(values1[i], true));
    lits3.push(genLit(values2[i], true));
    lits3.push(genLit(max + i, false));
    lits4.push(genLit(values1[i], true));
    lits4.push(genLit(values2[i], false));
    lits4.push(genLit(max + i, true));
    lits1.push(genLit(flag, true)); lits2.push(genLit(flag, true)); lits3.push(genLit(flag, true)); lits4.push(genLit(flag, true));
    solver->addClause_(lits1);
    solver->addClause_(lits2);
    solver->addClause_(lits3);
    solver->addClause_(lits4);
  }

  vec<Lit> tmp;
  for (int i = 0; i < values1.size(); i++)
    tmp.push(genLit(max + i, true));
  tmp.push(genLit(flag, true));
  solver->addClause_(tmp);
  return;
}

void GInterface::ifFalse_thenEQ(std::vector<int> values1, std::vector<int> values2, int flag) {
  //assert(values1.size() == values2.size());
  for (int i = 0; i < values1.size(); i++) {
    vec<Lit> tmp;
    tmp.push(genLit(values1[i], true));
    tmp.push(genLit(values2[i], false));
    tmp.push(genLit(flag, true));
    solver->addClause(tmp);
    tmp.clear();
    tmp.push(genLit(values1[i], false));
    tmp.push(genLit(values2[i], true));
    tmp.push(genLit(flag, true));
    solver->addClause(tmp);
  }
}

void GInterface::ifTrue_thenEQ(std::vector<int> values1, std::vector<int> values2, int flag) {
  //assert(values1.size() == values2.size());
  for (int i = 0; i < values1.size(); i++) {
    vec<Lit> tmp;
    tmp.push(genLit(values1[i], true));
    tmp.push(genLit(values2[i], false));
    tmp.push(genLit(flag, false));
    solver->addClause(tmp);
    tmp.clear();
    tmp.push(genLit(values1[i], false));
    tmp.push(genLit(values2[i], true));
    tmp.push(genLit(flag, false));
    solver->addClause(tmp);
  }
}

lbool GInterface::modelNum(int num) {
  return solver->model[num];
}

void GInterface::readClause2(StreamBuffer& in, vec<Lit>& lits, int offset) {
  lits.clear();
  for (;;) {
    int parsed_lit = parseInt(in);
    //std::cout << "parsed lit: " << parsed_lit << std::endl;
    if (parsed_lit == 0)
      break;
    int var = abs(parsed_lit) + offset;

    while (solver->nVars() <= var) {
      solver->newVar();
    }

    lits.push((parsed_lit > 0) ? mkLit(var) : ~mkLit(var));
  }
}

bool GInterface::getAnswer(const bool VERBOSE) {
  vec<Lit> dummy;
  std::chrono::high_resolution_clock::time_point start, end;
  solver->parsing = 0;
  solver->verbosity = 0;

  if (VERBOSE)
    start = std::chrono::high_resolution_clock::now();
  lbool res = solver->solveLimited(dummy);
  if (VERBOSE) {
    end = std::chrono::high_resolution_clock::now();
    double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }

  solver->parsing = 1;

  if (VERBOSE)
    std::cout << "#SOLVER vars:" << solver->nVars() << ", clauses: "
        << solver->nClauses() << ", time: " << time << "ms" << std::endl;

  return (res==l__True)?true:false;
}

bool GInterface::getAnswer(const vec<Lit>& assump,const bool VERBOSE) {
  std::chrono::high_resolution_clock::time_point start, end;
  solver->parsing = 0;
  solver->verbosity = 0;

  if (VERBOSE) {
    start = std::chrono::high_resolution_clock::now();
  }
  lbool res = solver->solveLimited(assump);
  if (VERBOSE) {
    end = std::chrono::high_resolution_clock::now();
    double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }
  solver->parsing = 1;

  if (VERBOSE)
    std::cout << "#SOLVER vars:" << solver->nVars() << ", clauses: "
    << solver->nClauses() << ", time: " << time << "ms" << std::endl;

  return (res==l__True)?true:false;
}

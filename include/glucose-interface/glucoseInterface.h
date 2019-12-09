#ifndef GLUCOSE_INTERFACE_H_
#define GLUCOSE_INTERFACE_H_

#include <map>
#include <vector>

#include "utils/System.h"
#include "core/Dimacs.h"
#include "simp/SimpSolver.h"
#include "CNFGenerator.h"

typedef std::map<std::string, unsigned int> Model;

namespace Glucose {

  class GInterface {
    public:
      SimpSolver* solver;
      ////////////////////////
	  void initSolver();
      GInterface();
      //~GInterface();
      GInterface(const GInterface &obj);
      bool getAnswer(bool VERBOSE = false);
      bool getAnswer(const vec<Lit>& assump, bool VERBOSE=false);
      lbool modelNum(int num);
      void addDimacs(const char* dimacsFile, int offset);
      void addCNF(CNFgen::CNFGenerator *cnf, int offset);
	  void addClause(int x, bool b) { solver->addClause(genLit(x, b)); };
      void setEqual(int a, int b);
      void setEqual(std::vector<int> values1, std::vector<int> values2);
      void setNEqual(std::vector<int> values1, std::vector<int> values2);

      void ifTrue_thenEQ_elseNEQ(std::vector<int> values1, std::vector<int> values2, int flag);
      void ifTrue_thenNEQ_elseEQ(std::vector<int> values1, std::vector<int> values2, int flag);

      void ifTrue_thenEQ(std::vector<int> values1, std::vector<int> values2, int flag);
      void ifFalse_thenEQ(std::vector<int> values1, std::vector<int> values2, int flag);
      void ifTrue_thenNEQ(std::vector<int> values1, std::vector<int> values2, int flag);
      void ifFalse_thenNEQ(std::vector<int> values1, std::vector<int> values2, int flag);

      Lit genLit(int x, bool b);

    private:
      const char* dimacsFile;
      /////////////////////
      void readClause2(StreamBuffer& in, vec<Lit>& lits, int offset);


  };

}
#endif

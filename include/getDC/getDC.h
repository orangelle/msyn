#ifndef GETDC_H_
#define GETDC_H_

#include <vector>
#include <string>
#include <set>
#include "node.h"
#include "glucoseInterface.h"

namespace DC {
  
class getDC {
public:
  enum FType{
      NOTHING = 0,
      IN1 = 1,
      IN2 = 2,
  };


  getDC(nodecircuit::NodeVector* arg_signals, nodecircuit::Node* arg_target, CNFgen::CNFGenerator* arg_fcnf, CNFgen::CNFGenerator* arg_gcnf, nodecircuit::Circuit* arg_impl);
  getDC(nodecircuit::NodeVector *arg_signals, nodecircuit::NodeVector *arg_targets, CNFgen::CNFGenerator *arg_fcnf, CNFgen::CNFGenerator *arg_gcnf, nodecircuit::Circuit *arg_impl);
  void removeCase(std::map < nodecircuit::Node*, bool > *data, FType type=NOTHING);
  void removeCases_fromFile(std::string filename, FType type);
  void solve();
  void solve_multi();
  int solve_additional();
  int solve_new();
  void dumpCases();
  //void dumpCases2files(std::string in1, std::string in2);
  
  std::vector< std::map < nodecircuit::Node*, bool > > in1cases;
  std::vector< std::map < nodecircuit::Node*, bool > > in2cases;

  std::vector< std::map < nodecircuit::Node*, bool > > dccases;
  std::vector< std::map < nodecircuit::Node*, bool > > in1dccases;
  std::vector< std::map < nodecircuit::Node*, bool > > in2dccases;

  std::set<int> removed_cases; // ADDED by Amir

private:
  nodecircuit::Circuit *circuit;
  CNFgen::CNFGenerator *fcnf;
  CNFgen::CNFGenerator *gcnf;
  nodecircuit::NodeVector *signals;
  nodecircuit::Node *target;
  nodecircuit::NodeVector *targets;
  Glucose::GInterface main;
  Glucose::GInterface check;
  static void Timeout(int signum);

  const int MAXTIME = 30;
};

}
#endif

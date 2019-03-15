#ifndef GENERATE_PATCH_BLIF_H
#define GENERATE_PATCH_BLIF_H

#include <vector>
#include <string>
#include "node.h"
#include "base/main/mainInt.h"
#include "base/main/main.h"
#include "base/abc/abc.h"
#include "map/mio/mio.h"

namespace patchgen {
  class Target_data {
  public:
    std::string name;//t_? (MEMO:If method of multiple change to use C???, this will be vector of target name.)
    std::vector<nodecircuit::Node*> signals;//n_?
    std::vector< std::map< nodecircuit::Node*, bool> > inxcases;//on_set
    std::map<std::string, bool> constraints;//like ["t_0",1]. It means t_0 = 1. (MEMO:If use C???, this will be C???.)
    
    Target_data(std::string target_name, std::vector< std::map< nodecircuit::Node*, bool> > target_inxcases);
    Target_data(std::string target_name, std::vector< std::map< nodecircuit::Node*, bool> > target_inxcases, std::map<std::string, bool> target_constraints);
    void set_signals();
    std::string get_node_string();
    int get_node(ABC::Abc_Ntk_t *pNtk);
  };

  class Adapter_data {
  public:
    Adapter_data(nodecircuit::NodeVector &new_solution, nodecircuit::NodeVector &initial_solution);
    
    virtual ~Adapter_data() = default;

    nodecircuit::NodeVector inputs;
    nodecircuit::NodeVector outputs;
    std::string file_name;

  private:
    void GetFaninCone(nodecircuit::Node *node, nodecircuit::NodeSet &fanin_nodes);
  };

  class Patch_data {
  public:
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> net;
    ABC::Abc_Ntk_t *pNtk;
    ABC::Abc_Frame_t *pAbc;
    Adapter_data* adapter_data;
    
    Patch_data(std::vector< std::vector<Target_data> > target_groups);
    
    void generate_blif(std::string filename);
    void generate_verilog(std::string filename);
  };
  
  void generate_out(std::string file_impl, std::string file_out, Patch_data patch_data);
  void read_library();
}

#endif

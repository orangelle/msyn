//
// Created by iwata on 17/09/03.
//

#ifndef DEPAG_MINCUT_H
#define DEPAG_MINCUT_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include "node.h"

namespace mincut {
  struct VertexProperty {
    std::string name;
    nodecircuit::Node *node;
  };
  typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
  typedef boost::labeled_graph<
    boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty,
                          boost::property<boost::edge_capacity_t, int,
                            boost::property<boost::edge_residual_capacity_t, int,
                              boost::property<boost::edge_reverse_t, Traits::edge_descriptor>>>>,
    std::string> Graph;

  class MinCut {
  public:
    MinCut() = default;

    virtual ~MinCut() = default;

    // Input is original patch inputs, and output is patch inputs optimized by Maxflow-Mincut theorem.
    // Return Maxflow value i.e. total weight of optimized solution.
    int GetMinCut(nodecircuit::NodeVector &input_solution, nodecircuit::NodeVector &optimized_solution);

  private:
    void AddEdge(nodecircuit::Node &node, nodecircuit::NodeSet &fanin_cone, nodecircuit::NodeSet &input_set,
                 boost::property_map<Graph, boost::edge_reverse_t>::type &rev, Traits::vertex_descriptor sink, Graph &g);

    void AddEdge_(Traits::vertex_descriptor v1, Traits::vertex_descriptor v2,
                 boost::property_map<Graph, boost::edge_reverse_t>::type &rev,
                 int capacity, Graph &g);

    void SearchForMinCut(nodecircuit::NodeSet &start, std::vector<boost::default_color_type> &color, Graph &g, nodecircuit::NodeVector &optimized);
  };
}
#endif //DEPAG_MINCUT_H

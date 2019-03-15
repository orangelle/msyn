//
// Created by iwata on 17/09/03.
//
#include <iostream>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include "mincut.h"

using namespace mincut;
using nodecircuit::Circuit;
using nodecircuit::NodeVector;
using nodecircuit::NodeSet;
using nodecircuit::Node;
using namespace boost;

int MinCut::GetMinCut(NodeVector &input, NodeVector &optimized) {
  NodeSet input_set(input.begin(), input.end());
  NodeSet fanin_cone;
  NodeSet fanin_pi;
  Circuit dummy;
  for (auto &&node: input_set) {
    dummy.GetFaninCone(node, fanin_cone, fanin_pi);
    if (node->is_input) {
      fanin_pi.insert(node);
    }
  }

  Graph g;

  // Create Vertices
  Traits::vertex_descriptor src = g.add_vertex(std::string("src_"));
  Traits::vertex_descriptor sink = g.add_vertex(std::string("sink_"));
  g[std::string("src_")].name = std::string("src_");
  g[std::string("src_")].node = nullptr;
  g[std::string("sink_")].name = std::string("sink_");
  g[std::string("sink_")].node = nullptr;
  for (auto &&node : fanin_pi) {
    g.add_vertex(node->name);
    g[node->name].name = node->name;
    g[node->name].node = node;
  }
  for (auto &&node : fanin_cone) {
    g.add_vertex(node->name);
    g[node->name].name = node->name;
    g[node->name].node = node;
  }
  for (auto &&node : input_set) {
    g.add_vertex(node->name);
    g[node->name].name = node->name;
    g[node->name].node = node;
  }

  // Create Edges
  property_map<Graph, edge_reverse_t>::type rev = get(edge_reverse, g);
  for (auto &&node : fanin_pi) {
    AddEdge_(src, g.vertex(node->name), rev, node->weight, g);
    AddEdge(*node, fanin_cone, input_set, rev, sink, g);
  }
  for (auto &&node : fanin_cone) {
    AddEdge(*node, fanin_cone, input_set, rev, sink, g);
  }
  for (auto &&node : input_set) {
    if (fanin_pi.count(node) > 0 or fanin_cone.count(node) > 0) {
      continue;
    }
    AddEdge_(g.vertex(node->name), sink, rev, node->weight, g);
  }

  int input_weight = 0;
  for (auto &&node : input_set) {
    input_weight += node->weight;
  }
  std::cout << "Weight of original: " << input_weight << std::endl;

  // Execute MaxFlow-MinCut algorithm.
  std::vector<default_color_type> color(num_vertices(g));
  int flow = boykov_kolmogorov_max_flow(g, get(edge_capacity, g), get(edge_residual_capacity, g), rev, &color[0], get(vertex_index, g), src, sink);
  std::cout << "Weight of mincut: " << flow << std::endl;

  // Search for mincut
  for (auto &&node : fanin_pi) {
    color[g.vertex(node->name)] = color[src];
  }
  SearchForMinCut(fanin_pi, color, g, optimized);

  int optimized_weight = 0;
  for (auto &&node : optimized) {
    optimized_weight += node->weight;
  }
  if (flow != optimized_weight) {
    std::cerr << "Failed to find mincut." << std::endl;
  }
  return optimized_weight;
}


void MinCut::AddEdge(Node &node, NodeSet &fanin_cone, NodeSet &input_set,
                     property_map<Graph, edge_reverse_t>::type &rev, Traits::vertex_descriptor sink, Graph &g) {
  NodeVector valid_fanout;
  for (auto &&fanout : node.outputs) {
    if (fanin_cone.count(fanout) > 0 or input_set.count(fanout) > 0) {
      valid_fanout.push_back(fanout);
    }
  }

  if (input_set.count(&node) > 0) { // A node which is in original patch inputs
    std::string buffer = node.name + "_buf";
    g.add_vertex(buffer);
    g[buffer].name = buffer;
    g[buffer].node = &node;
    AddEdge_(g.vertex(node.name), g.vertex(buffer), rev, node.weight, g);
    AddEdge_(g.vertex(buffer), sink, rev, node.weight, g);
    for (auto &&fanout : valid_fanout) {
      AddEdge_(g.vertex(buffer), g.vertex(fanout->name), rev, node.weight, g);
    }
  } else if (valid_fanout.size() == 1) {
    AddEdge_(g.vertex(node.name), g.vertex(valid_fanout[0]->name), rev, node.weight, g);
  } else if (valid_fanout.size() > 1) {
    std::string buffer = node.name + "_buf";
    g.add_vertex(buffer);
    g[buffer].name = buffer;
    g[buffer].node = &node;
    AddEdge_(g.vertex(node.name), g.vertex(buffer), rev, node.weight, g);
    for (auto &&fanout : valid_fanout) {
      AddEdge_(g.vertex(buffer), g.vertex(fanout->name), rev, node.weight, g);
    }
  }
}


void MinCut::AddEdge_(Traits::vertex_descriptor v1, Traits::vertex_descriptor v2,
                     property_map<Graph, edge_reverse_t>::type &rev, const int capacity, Graph &g) {
  Traits::edge_descriptor e1 = add_edge(v1, v2, g).first;
  Traits::edge_descriptor e2 = add_edge(v2, v1, g).first;
  put(edge_capacity, g, e1, capacity);
  put(edge_capacity, g, e2, 0);
  rev[e1] = e2;
  rev[e2] = e1;
}


void MinCut::SearchForMinCut(NodeSet &start, std::vector<default_color_type> &color, Graph &g, NodeVector &optimized) {
  property_map<Graph, edge_capacity_t>::type capacity = get(edge_capacity, g);
  std::set<Traits::vertex_descriptor> visited;
  std::deque<Traits::vertex_descriptor> dq;

  for (auto &&node : start) {
    visited.insert(g.vertex(node->name));
    dq.push_back(g.vertex(node->name));
  }

  while (!dq.empty()) {
    Traits::vertex_descriptor ver = dq.front();
    dq.pop_front();
    graph_traits<Graph>::out_edge_iterator ei, e_end;
    for (tie(ei, e_end) = out_edges(ver, g); ei != e_end; ++ei) {
      if (capacity[*ei] > 0 and color[target(*ei, g)] != color[source(*ei, g)]) {
        optimized.push_back(g.graph()[ver].node);
        break;
      }
    }

    for (tie(ei, e_end) = out_edges(ver, g); ei != e_end; ++ei) {
      if (capacity[*ei] > 0 and visited.count(target(*ei, g)) == 0 and color[target(*ei, g)] == color[source(*ei, g)]) {
        visited.insert(target(*ei, g));
        dq.push_back(target(*ei, g));
      }
    }
  }
}

#ifndef LOGICOPT_CIRCUIT_NODE_H
#define LOGICOPT_CIRCUIT_NODE_H

#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <map>

namespace nodecircuit {

	class Node;
	class Cluster;

	typedef std::vector<Node *> NodeVector;
	typedef std::set<Node *> NodeSet;
	typedef std::vector<Cluster*> ClusterVector;
	typedef std::set<Cluster*> ClusterSet;
	typedef std::unordered_set<Cluster*> ClusterUnset;

	class Node {
	public:
		Node() {
			is_input = false;
			is_output = false;
			weight = 1;
			weight_for_elim = 0;
			order = 0;
		}
		virtual ~Node() {}

	public:
		std::string cube;
		std::string cube_pre;
		std::string name;
		bool is_input;
		bool is_output;
		NodeVector inputs;  // fanins  of the node
		NodeVector inputs_pre;
		NodeVector outputs; // fanouts of the node
		Cluster* my_cluster;
		int order;
		int weight;
		int weight_for_elim;  // used to eliminate candidates for inputs of target
		bool HasOutput() {
			return !outputs.empty();
		}
		//// get the non controlling node
		//Node* GetNonControlNode(Node* zero_node, Node* one_node);
		//// get the controlling node
		//Node* GetControlNode(Node* zero_node, Node* one_node);
	};

	class Cluster {
	public:
		Cluster() {};
		~Cluster() {};

		void append(Node* node) {
			if (name.empty())
				name = node->name;
			nodes_c.push_back(node);
			//output_nodes.insert(node->outputs.begin(), node->outputs.end());
			input_nodes.insert(node->inputs.begin(), node->inputs.end());
			node->my_cluster = this;
			for (Node* nd : nodes_c) 
				if (input_nodes.find(nd) != input_nodes.end())
					input_nodes.erase(nd);
		};
		std::string name;
		NodeVector nodes_c;
		//NodeSet output_nodes;
		NodeSet input_nodes_pre;
		NodeSet input_nodes;
		//std::vector<Cluster*> output_clusters;
		//std::vector<Cluster*> input_clusters;
	};

	class Circuit {
	public:
		Circuit() {};
		virtual ~Circuit() {};

		// write as a get-level verilog
		//int WriteVerilog(std::string filename);

		// write as a blif
		int WriteBlif(std::string filename);
		int WriteBlif_m(std::string filename);
		// load the weights fromm the given file
		int ReadWeights(std::string filename);

		// remove orphan nodes
		int PurgeCircuit();
		//assign order
		int AssignOrder();
		//sort the nodes in different ways
		int SortByOrder();
		int SortByFanout();
		int SortByFanin();
		//Initially creat a cluster for every node except PIs
		int InitCluster();
		//merge clu2 to clu1
		ClusterUnset::iterator Merge(Cluster* clu1, Cluster* clu2);
		//Get the cluster automatically by trivial heuristic
		int GetCluster(int max, int thred, bool by_input);
		//Get the maximum fanout sharedness of a node for each node of the cluster
		int Sharedness(Node* node, Cluster* cluster, bool by_input);
		// get the fanin cone (and fanin primary inputs) of a cluster
		int GetFaninCone(Cluster* clst, ClusterSet& fanin_clsts);
		// get the fanout cone (and fanout primary outputs) of a node
		int GetFanoutCone(Cluster* clst, ClusterSet& fanout_clsts);
		// get the fanin cone (and fanin primary inputs) of a node
		int GetFaninCone(Node* node, NodeSet& fanin_nodes, NodeSet& fanin_pi);
		// get the fanout cone (and fanout primary outputs) of a node
		int GetFanoutCone(Node* node, NodeSet& fanout_nodes, NodeSet& fanout_po);
		// get the fanin primary inputs of a node
		int GetFaninPi(Node* node, NodeSet& fanin_pi) {
			NodeSet fanin_nodes;
			return GetFaninCone(node, fanin_nodes, fanin_pi);
		}
		// get the fanout primary outputs of a node
		int GetFanoutPo(Node* node, NodeSet& fanout_po) {
			NodeSet fanout_nodes;
			return GetFanoutCone(node, fanout_nodes, fanout_po);
		}
		// get fanout cone of the input nodes, excluding the nodes specified
		int GetFanoutCone(NodeSet& input_nodes, NodeSet& fanout_nodes, NodeSet& exclude_nodes);
		// convert the nodes to targets
		int SetTargets(std::vector<std::string> node_names);
		int SetTargets(NodeVector nodes);
		// randomly select n nodes in fanin cone
		int SelectTargets(std::vector<std::string>& node_names , Node* node , int n);
		// restruct the circuit
		int Restruct(Node* target, std::vector< std::map < Node*, bool>> in2cases);
		int Restruct(Node* target_impl);
		// remove the node
		int Remove(Node* node);
		// restore the circuit
		int Restore(NodeVector& targets);
		// judge whether nodes_new is in nodes_old's TFO
		int IsInFanoutCone(NodeVector nodes_old, NodeVector nodes_new);

		std::string name;
		NodeVector inputs;    // primary inputs
		NodeVector outputs;   // primary outputs
		NodeVector targets;   // targets of debug
		NodeVector all_nodes; // all nodes including inputs/outputs/targets
		//ClusterVector all_clusters;
		ClusterUnset all_clusters;
		// mapping node names to nodes
		std::map<std::string, Node *> all_nodes_map;
		// find a node by name, returns NULL if not found
		Node* GetNode(std::string name) {
			std::map<std::string, Node*>::iterator it = all_nodes_map.find(name);
			if (it != all_nodes_map.end())
				return it->second;
			return NULL;
		}
	};

} // namespace nodecircuit

#endif //PARSER_NODE_H

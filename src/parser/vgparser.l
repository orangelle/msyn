// simple verilog gate-level parser, without error recover, and without comment detection

%{
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;
%}

%top{
#include "node.h"
using namespace nodecircuit;
%}

%class{
Node* current_node;
int mode;
virtual int wrap() { return 1; }
public:
Circuit* circuit;
%}

%init{
circuit = NULL;
current_node = NULL;
mode = 255; // 128:start 129:output 130:input 131:wire 132:gateinput 254:nothing 255:finish others:gate
%}

newline  \r?\n
space [ \t\r\n]
id  [a-zA-Z][a-zA-Z0-9_]*
zero 1'b0
one 1'b1

%option fast
%option namespace=veriloggateparser

%%

"endmodule"    mode = 255;
"module"       mode = 128;
"output"       mode = 129;
"input"        mode = 130;
"wire"         mode = 131;
"buff"         mode = NODE_BUF;
"buf"          mode = NODE_BUF;
"not"          mode = NODE_NOT;
"nand"         mode = NODE_NAND;
"and"          mode = NODE_AND;
"xnor"         mode = NODE_XNOR;
"xor"          mode = NODE_XOR;
"nor"          mode = NODE_NOR;
"or"           mode = NODE_OR;


{id}	switch (mode) {
          case 128:
            circuit->name = text();
            mode = 254;
            break;
          case 129:
            current_node = new Node;
            current_node->name = text();
            current_node->is_output = true;;
            circuit->outputs.push_back(current_node);
            circuit->all_nodes.push_back(current_node);
            circuit->all_nodes_map[current_node->name] = current_node;
            break;
          case 130:
            current_node = new Node;
            current_node->name = text();
            current_node->is_input = true;;
            circuit->inputs.push_back(current_node);
            circuit->all_nodes.push_back(current_node);
            circuit->all_nodes_map[current_node->name] = current_node;
            break;
          case 131:
            current_node = new Node;
            current_node->name = text();
            if (current_node->name[0] == 't' && current_node->name[1] == '_')
              circuit->targets.push_back(current_node);
            circuit->all_nodes.push_back(current_node);
            circuit->all_nodes_map[current_node->name] = current_node;
            break;
          case NODE_BUF:
          case NODE_NOT:
          case NODE_AND:
          case NODE_NAND:
          case NODE_OR:
          case NODE_NOR:
          case NODE_XOR:
          case NODE_XNOR:
            current_node = circuit->all_nodes_map[string(text())];
            current_node->type = (NodeType)mode;
            mode = 132;
            break;
          case 132:
            Node* node = circuit->all_nodes_map[string(text())];
            current_node->inputs.push_back(node);
            node->outputs.push_back(current_node);
            break;
        }

{zero}  if (mode == 132) {
          Node* node = NULL;
          if (circuit->all_nodes_map.find("zero") != circuit->all_nodes_map.end()) {
            node = circuit->all_nodes_map["zero"];
          }
          else {
            node = new Node;
            node->name = "zero";
            node->is_input = true;
            circuit->all_nodes.push_back(node);
            circuit->all_nodes_map["zero"] = node;
          }   
          current_node->inputs.push_back(node);
          node->outputs.push_back(current_node);
        }

{one}  if (mode == 132) {
          Node* node = NULL;
          if (circuit->all_nodes_map.find("one") != circuit->all_nodes_map.end()) {
            node = circuit->all_nodes_map["one"];
          }
          else {
            node = new Node;
            node->name = "one";
            node->is_input = true;
            circuit->all_nodes.push_back(node);
            circuit->all_nodes_map["one"] = node;
          }   
          current_node->inputs.push_back(node);
          node->outputs.push_back(current_node);
        }


.	mode=mode;

%%

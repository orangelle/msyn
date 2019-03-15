%{
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;
%}

%top{
#include "node.h"
using namespace nodecircuit;
%}

%class{
Node* current_node;
NodeVector nodes;
int mode;
virtual int wrap() { return 1; }
public:
Circuit* circuit;
%}

%init{
circuit = NULL;
current_node = NULL;
mode = 254;
%}

newline \r?\n
array [01-]*[ \t][01]
id [a-zA-Z][a-zA-z0-9_]*

%option fast
%option namespace=veriloggateparser

%%

".end" mode = 255;
".model" mode = 128;
".outputs" mode = 129;
".inputs" mode = 130;
".names" mode = 131;

{id}    switch (mode){
            case 128:
                circuit->name = text();
                mode = 254;
                break;
            case 129:
                current_node = new Node;
                current_node->name = text();
                current_node->is_output = true;
                circuit->outputs.push_back(current_node);
                circuit->all_nodes.push_back(current_node);
                circuit->all_nodes_map[current_node->name] = current_node;
                break;
            case 130:
                current_node = new Node;
                current_node->name = text();
                current_node->is_input = true;
                circuit->inputs.push_back(current_node);
                circuit->all_nodes.push_back(current_node);
                circuit->all_nodes_map[current_node->name] = current_node;
                break;
            case 131:
                map<string, Node*>::iterator it = circuit->all_nodes_map.find(text());
                if (it == circuit->all_nodes_map.end()){
                    current_node = new Node;
                    current_node->name = text();
                    circuit->all_nodes.push_back(current_node);
                    circuit->all_nodes_map[current_node->name] = current_node;
                }
                else
                    current_node = it->second;
                nodes.push_back(current_node);
                break;
        }

{newline}  if(mode == 131){ 
                for(auto input_node:nodes){
                    if(current_node!=input_node){
                        current_node->inputs.push_back(input_node);
                        input_node->outputs.push_back(current_node);
                    }
                }
                nodes.clear();
                mode = 132;
            }

{array}     if(mode == 132){string line = text();
            current_node->cube += line;
            current_node->cube += "\n";}
            
.   mode = mode;
%%

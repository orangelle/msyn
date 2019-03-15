// ./src/vgparser/vgparser.cpp generated by reflex 0.9.22 from ./src/vgparser/vgparser.l

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define REFLEX_OPTION_fast                true
#define REFLEX_OPTION_header_file         ./include/vgparser/vgparser.h
#define REFLEX_OPTION_lex                 lex
#define REFLEX_OPTION_lexer               Lexer
#define REFLEX_OPTION_namespace           veriloggateparser
#define REFLEX_OPTION_outfile             ./src/vgparser/vgparser.cpp

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top{ user code %}                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 12 "./src/vgparser/vgparser.l"

#include "node.h"
using namespace nodecircuit;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  REGEX MATCHER                                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/matcher.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ABSTRACT LEXER CLASS                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/abslexer.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  LEXER CLASS                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace veriloggateparser {

class Lexer : public reflex::AbstractLexer<reflex::Matcher> {
#line 17 "./src/vgparser/vgparser.l"

Node* current_node;
int mode;
virtual int wrap() { return 1; }
public:
Circuit* circuit;

 public:
  typedef reflex::AbstractLexer<reflex::Matcher> AbstractBaseLexer;
  Lexer(
      const reflex::Input& input = reflex::Input(),
      std::ostream&        os    = std::cout)
    :
      AbstractBaseLexer(input, os)
  {
#line 25 "./src/vgparser/vgparser.l"

circuit = NULL;
current_node = NULL;
mode = 255; // 128:start 129:output 130:input 131:wire 132:gateinput 254:nothing 255:finish others:gate

  }
  static const int INITIAL = 0;
  virtual int lex();
  int lex(
      const reflex::Input& input,
      std::ostream        *os = NULL)
  {
    in(input);
    if (os)
      out(*os);
    return lex();
  }
};

}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %{ user code %}                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 1 "./src/vgparser/vgparser.l"
// simple verilog gate-level parser, without error recover, and without comment detection


#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 2: rules                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

extern void reflex_code_INITIAL(reflex::Matcher&);

int veriloggateparser::Lexer::lex()
{
  static const reflex::Pattern PATTERN_INITIAL(reflex_code_INITIAL);
  if (!has_matcher())
  {
    matcher(new Matcher(PATTERN_INITIAL, stdinit(), this));
  }
  while (true)
  {
        switch (matcher().scan())
        {
          case 0:
            if (matcher().at_end())
            {
              return 0;
            }
            else
            {
              out().put(matcher().input());
            }
            break;
          case 1: // rule at line 42: (?:\Qendmodule\E)
#line 42 "./src/vgparser/vgparser.l"
mode = 255;
            break;
          case 2: // rule at line 43: (?:\Qmodule\E)
#line 43 "./src/vgparser/vgparser.l"
mode = 128;
            break;
          case 3: // rule at line 44: (?:\Qoutput\E)
#line 44 "./src/vgparser/vgparser.l"
mode = 129;
            break;
          case 4: // rule at line 45: (?:\Qinput\E)
#line 45 "./src/vgparser/vgparser.l"
mode = 130;
            break;
          case 5: // rule at line 46: (?:\Qwire\E)
#line 46 "./src/vgparser/vgparser.l"
mode = 131;
            break;
          case 6: // rule at line 47: (?:\Qbuff\E)
#line 47 "./src/vgparser/vgparser.l"
mode = NODE_BUF;
            break;
          case 7: // rule at line 48: (?:\Qbuf\E)
#line 48 "./src/vgparser/vgparser.l"
mode = NODE_BUF;
            break;
          case 8: // rule at line 49: (?:\Qnot\E)
#line 49 "./src/vgparser/vgparser.l"
mode = NODE_NOT;
            break;
          case 9: // rule at line 50: (?:\Qnand\E)
#line 50 "./src/vgparser/vgparser.l"
mode = NODE_NAND;
            break;
          case 10: // rule at line 51: (?:\Qand\E)
#line 51 "./src/vgparser/vgparser.l"
mode = NODE_AND;
            break;
          case 11: // rule at line 52: (?:\Qxnor\E)
#line 52 "./src/vgparser/vgparser.l"
mode = NODE_XNOR;
            break;
          case 12: // rule at line 53: (?:\Qxor\E)
#line 53 "./src/vgparser/vgparser.l"
mode = NODE_XOR;
            break;
          case 13: // rule at line 54: (?:\Qnor\E)
#line 54 "./src/vgparser/vgparser.l"
mode = NODE_NOR;
            break;
          case 14: // rule at line 55: (?:\Qor\E)
#line 55 "./src/vgparser/vgparser.l"
mode = NODE_OR;


            break;
          case 15: // rule at line 58: (?:[A-Za-z][0-9A-Z_a-z]*)
#line 58 "./src/vgparser/vgparser.l"
switch (mode) {
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

            break;
          case 16: // rule at line 106: (?:1'b0)
#line 106 "./src/vgparser/vgparser.l"
if (mode == 132) {
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

            break;
          case 17: // rule at line 122: (?:1'b1)
#line 122 "./src/vgparser/vgparser.l"
if (mode == 132) {
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


            break;
          case 18: // rule at line 139: .
#line 139 "./src/vgparser/vgparser.l"
mode=mode;

            break;
        }
  }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  TABLES                                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/matcher.h>

void reflex_code_INITIAL(reflex::Matcher& m)
{
  int c0 = 0, c1 = c0;
  m.FSM_INIT(c1);

S0:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'x') goto S73;
  if (c1 == 'w') goto S44;
  if (c1 == 'o') goto S29;
  if (c1 == 'n') goto S58;
  if (c1 == 'm') goto S22;
  if (c1 == 'i') goto S37;
  if (c1 == 'e') goto S15;
  if ('c' <= c1 && c1 <= 'z') goto S81;
  if (c1 == 'b') goto S51;
  if (c1 == 'a') goto S66;
  if ('A' <= c1 && c1 <= 'Z') goto S81;
  if (c1 == '1') goto S87;
  if ('\v' <= c1) goto S90;
  if ('\n' <= c1) return m.FSM_HALT(c1);
  goto S90;

S15:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S92;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S22:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S105;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S29:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S112;
  if (c1 == 'r') goto S119;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S37:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S125;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S44:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'i') goto S132;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S51:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S139;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S58:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S146;
  if ('b' <= c1 && c1 <= 'z') goto S99;
  if (c1 == 'a') goto S154;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S66:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S161;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S73:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S175;
  if (c1 == 'n') goto S168;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S81:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S87:
  m.FSM_TAKE(18);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == '\'') goto S182;
  return m.FSM_HALT(c1);

S90:
  m.FSM_TAKE(18);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S92:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'd') goto S184;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S99:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S105:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'd') goto S191;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S112:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 't') goto S198;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S119:
  m.FSM_TAKE(14);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S125:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'p') goto S205;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S132:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S212;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S139:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'f') goto S219;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S146:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 't') goto S226;
  if (c1 == 'r') goto S232;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S154:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S238;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S161:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'd') goto S245;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S168:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S251;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S175:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S258;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S182:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'b') goto S264;
  return m.FSM_HALT(c1);

S184:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'm') goto S267;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S191:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S274;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S198:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'p') goto S281;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S205:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S288;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S212:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S295;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S219:
  m.FSM_TAKE(7);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'f') goto S301;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S226:
  m.FSM_TAKE(8);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S232:
  m.FSM_TAKE(13);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S238:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'd') goto S307;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S245:
  m.FSM_TAKE(10);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S251:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S313;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S258:
  m.FSM_TAKE(12);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S264:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == '1') goto S321;
  if (c1 == '0') goto S319;
  return m.FSM_HALT(c1);

S267:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S323;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S274:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'l') goto S330;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S281:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S337;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S288:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 't') goto S344;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S295:
  m.FSM_TAKE(5);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S301:
  m.FSM_TAKE(6);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S307:
  m.FSM_TAKE(9);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S313:
  m.FSM_TAKE(11);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S319:
  m.FSM_TAKE(16);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S321:
  m.FSM_TAKE(17);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S323:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'd') goto S350;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S330:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S357;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S337:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 't') goto S363;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S344:
  m.FSM_TAKE(4);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S350:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S369;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S357:
  m.FSM_TAKE(2);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S363:
  m.FSM_TAKE(3);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S369:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'l') goto S376;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S376:
  m.FSM_TAKE(15);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S383;
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);

S383:
  m.FSM_TAKE(1);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('a' <= c1 && c1 <= 'z') goto S99;
  if (c1 == '_') goto S99;
  if ('A' <= c1 && c1 <= 'Z') goto S99;
  if ('0' <= c1 && c1 <= '9') goto S99;
  return m.FSM_HALT(c1);
}


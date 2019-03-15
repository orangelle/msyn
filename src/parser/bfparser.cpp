// ./src/parser/bfparser.cpp generated by reflex 0.9.25 from ./src/parser/bfparser.l

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define REFLEX_OPTION_fast                true
#define REFLEX_OPTION_header_file         ./include/parser/bfparser.h
#define REFLEX_OPTION_lex                 lex
#define REFLEX_OPTION_lexer               Lexer
#define REFLEX_OPTION_namespace           veriloggateparser
#define REFLEX_OPTION_outfile             ./src/parser/bfparser.cpp

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top{ user code %}                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 9 "./src/parser/bfparser.l"

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
#line 14 "./src/parser/bfparser.l"

Node* current_node;
NodeVector nodes;
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
#line 23 "./src/parser/bfparser.l"

circuit = NULL;
current_node = NULL;
mode = 254;

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

#line 1 "./src/parser/bfparser.l"

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
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
          case 1: // rule at line 38: (?:\Q.end\E)
#line 38 "./src/parser/bfparser.l"
mode = 255;
            break;
          case 2: // rule at line 39: (?:\Q.model\E)
#line 39 "./src/parser/bfparser.l"
mode = 128;
            break;
          case 3: // rule at line 40: (?:\Q.outputs\E)
#line 40 "./src/parser/bfparser.l"
mode = 129;
            break;
          case 4: // rule at line 41: (?:\Q.inputs\E)
#line 41 "./src/parser/bfparser.l"
mode = 130;
            break;
          case 5: // rule at line 42: (?:\Q.names\E)
#line 42 "./src/parser/bfparser.l"
mode = 131;

            break;
          case 6: // rule at line 44: (?:[A-Za-z][0-9A-z]*)
#line 44 "./src/parser/bfparser.l"
switch (mode){
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

            break;
          case 7: // rule at line 79: (?:\r?\n)
#line 79 "./src/parser/bfparser.l"
if(mode == 131){
                for(auto input_node:nodes){
                    if(current_node!=input_node){
                        current_node->inputs.push_back(input_node);
                        input_node->outputs.push_back(current_node);
                    }
                }
                nodes.clear();
                mode = 132;
            }

            break;
          case 8: // rule at line 90: (?:[\x2d01]*[\x09\x20][01])
#line 90 "./src/parser/bfparser.l"
if(mode == 132){string line = text();
            current_node->cube += line;
            current_node->cube += "\n";}

            break;
          case 9: // rule at line 94: .
#line 94 "./src/parser/bfparser.l"
mode = mode;
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
  if ('a' <= c1 && c1 <= 'z') goto S17;
  if ('A' <= c1 && c1 <= 'Z') goto S17;
  if ('0' <= c1 && c1 <= '1') goto S31;
  if (c1 == '.') goto S10;
  if (c1 == '-') goto S31;
  if (c1 == ' ') goto S23;
  if (c1 == '\r') goto S28;
  if (c1 == '\n') goto S21;
  if (c1 == '\t') goto S23;
  if ('\0' <= c1) goto S26;
  return m.FSM_HALT(c1);

S10:
  m.FSM_TAKE(9);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S41;
  if (c1 == 'n') goto S45;
  if (c1 == 'm') goto S39;
  if (c1 == 'i') goto S43;
  if (c1 == 'e') goto S37;
  return m.FSM_HALT(c1);

S17:
  m.FSM_TAKE(6);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('A' <= c1 && c1 <= 'z') goto S47;
  if ('0' <= c1 && c1 <= '9') goto S47;
  return m.FSM_HALT(c1);

S21:
  m.FSM_TAKE(7);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S23:
  m.FSM_TAKE(9);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '1') goto S51;
  return m.FSM_HALT(c1);

S26:
  m.FSM_TAKE(9);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S28:
  m.FSM_TAKE(9);
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == '\n') goto S21;
  return m.FSM_HALT(c1);

S31:
  m.FSM_TAKE(9);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '1') goto S55;
  if (c1 == '-') goto S55;
  if (c1 == ' ') goto S53;
  if (c1 == '\t') goto S53;
  return m.FSM_HALT(c1);

S37:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S60;
  return m.FSM_HALT(c1);

S39:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S62;
  return m.FSM_HALT(c1);

S41:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S64;
  return m.FSM_HALT(c1);

S43:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S66;
  return m.FSM_HALT(c1);

S45:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'a') goto S68;
  return m.FSM_HALT(c1);

S47:
  m.FSM_TAKE(6);
  c0 = c1, c1 = m.FSM_CHAR();
  if ('A' <= c1 && c1 <= 'z') goto S47;
  if ('0' <= c1 && c1 <= '9') goto S47;
  return m.FSM_HALT(c1);

S51:
  m.FSM_TAKE(8);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S53:
  c0 = c1, c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '1') goto S51;
  return m.FSM_HALT(c1);

S55:
  c0 = c1, c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '1') goto S55;
  if (c1 == '-') goto S55;
  if (c1 == ' ') goto S53;
  if (c1 == '\t') goto S53;
  return m.FSM_HALT(c1);

S60:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'd') goto S70;
  return m.FSM_HALT(c1);

S62:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'd') goto S72;
  return m.FSM_HALT(c1);

S64:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 't') goto S74;
  return m.FSM_HALT(c1);

S66:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'p') goto S76;
  return m.FSM_HALT(c1);

S68:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'm') goto S78;
  return m.FSM_HALT(c1);

S70:
  m.FSM_TAKE(1);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S72:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S80;
  return m.FSM_HALT(c1);

S74:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'p') goto S82;
  return m.FSM_HALT(c1);

S76:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S84;
  return m.FSM_HALT(c1);

S78:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S86;
  return m.FSM_HALT(c1);

S80:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'l') goto S88;
  return m.FSM_HALT(c1);

S82:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S90;
  return m.FSM_HALT(c1);

S84:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 't') goto S92;
  return m.FSM_HALT(c1);

S86:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 's') goto S94;
  return m.FSM_HALT(c1);

S88:
  m.FSM_TAKE(2);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S90:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 't') goto S96;
  return m.FSM_HALT(c1);

S92:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 's') goto S98;
  return m.FSM_HALT(c1);

S94:
  m.FSM_TAKE(5);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S96:
  c0 = c1, c1 = m.FSM_CHAR();
  if (c1 == 's') goto S100;
  return m.FSM_HALT(c1);

S98:
  m.FSM_TAKE(4);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);

S100:
  m.FSM_TAKE(3);
  c0 = c1, c1 = m.FSM_CHAR();
  return m.FSM_HALT(c1);
}


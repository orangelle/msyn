#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <cmath>
#include <unordered_map>
//#include <cassert>

#include "patchgen.h"

using namespace patchgen;
using namespace nodecircuit;
char lib[19000] =
  "GATE NOT 1 Y=!A; PIN A INV 1 999 1 0 1 0\nGATE AND2 1 Y=IN0 * IN1; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0\nGATE NAND2 1 Y=!(IN0 * IN1); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0\nGATE OR2 1 Y=IN0 + IN1; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0\nGATE NOR2 1 Y=!(IN0 + IN1); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0\nGATE XOR2 1 Y=(IN0) * !IN1 + !(IN0) * IN1; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0\nGATE XNOR2 1 Y=!((IN0) * !IN1 + !(IN0) * IN1); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0\nGATE AND3 1 Y=IN0 * IN1 * IN2; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0\nGATE NAND3 1 Y=!(IN0 * IN1 * IN2); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0\nGATE OR3 1 Y=IN0 + IN1 + IN2; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0\nGATE NOR3 1 Y=!(IN0 + IN1 + IN2); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0\nGATE XOR3 1 Y=((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0\nGATE XNOR3 1 Y=!(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0\nGATE AND4 1 Y=IN0 * IN1 * IN2 * IN3; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0\nGATE NAND4 1 Y=!(IN0 * IN1 * IN2 * IN3); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0\nGATE OR4 1 Y=IN0 + IN1 + IN2 + IN3; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0\nGATE NOR4 1 Y=!(IN0 + IN1 + IN2 + IN3); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0\nGATE XOR4 1 Y=(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0\nGATE XNOR4 1 Y=!((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0\nGATE AND5 1 Y=IN0 * IN1 * IN2 * IN3 * IN4; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0\nGATE NAND5 1 Y=!(IN0 * IN1 * IN2 * IN3 * IN4); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0\nGATE OR5 1 Y=IN0 + IN1 + IN2 + IN3 + IN4; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0\nGATE NOR5 1 Y=!(IN0 + IN1 + IN2 + IN3 + IN4); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0\nGATE XOR5 1 Y=((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0\nGATE XNOR5 1 Y=!(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0\nGATE AND6 1 Y=IN0 * IN1 * IN2 * IN3 * IN4 * IN5; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0\nGATE NAND6 1 Y=!(IN0 * IN1 * IN2 * IN3 * IN4 * IN5); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0\nGATE OR6 1 Y=IN0 + IN1 + IN2 + IN3 + IN4 + IN5; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0\nGATE NOR6 1 Y=!(IN0 + IN1 + IN2 + IN3 + IN4 + IN5); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0\nGATE XOR6 1 Y=(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0\nGATE XNOR6 1 Y=!((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0\nGATE AND7 1 Y=IN0 * IN1 * IN2 * IN3 * IN4 * IN5 * IN6; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0\nGATE NAND7 1 Y=!(IN0 * IN1 * IN2 * IN3 * IN4 * IN5 * IN6); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0\nGATE OR7 1 Y=IN0 + IN1 + IN2 + IN3 + IN4 + IN5 + IN6; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0\nGATE NOR7 1 Y=!(IN0 + IN1 + IN2 + IN3 + IN4 + IN5 + IN6); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0\nGATE XOR7 1 Y=((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * !IN6 + !((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * IN6; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0\nGATE XNOR7 1 Y=!(((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * !IN6 + !((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * IN6); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0\nGATE AND8 1 Y=IN0 * IN1 * IN2 * IN3 * IN4 * IN5 * IN6 * IN7; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0 PIN IN7 INV 1 999 1 0 1 0\nGATE NAND8 1 Y=!(IN0 * IN1 * IN2 * IN3 * IN4 * IN5 * IN6 * IN7); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0 PIN IN7 INV 1 999 1 0 1 0\nGATE OR8 1 Y=IN0 + IN1 + IN2 + IN3 + IN4 + IN5 + IN6 + IN7; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0 PIN IN7 INV 1 999 1 0 1 0\nGATE NOR8 1 Y=!(IN0 + IN1 + IN2 + IN3 + IN4 + IN5 + IN6 + IN7); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0 PIN IN7 INV 1 999 1 0 1 0\nGATE XOR8 1 Y=(((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * !IN6 + !((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * IN6) * !IN7 + !(((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * !IN6 + !((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * IN6) * IN7; PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0 PIN IN7 INV 1 999 1 0 1 0\nGATE XNOR8 1 Y=!((((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * !IN6 + !((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * IN6) * !IN7 + !(((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * !IN6 + !((((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * !IN5 + !(((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * !IN4 + !((((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * !IN3 + !(((IN0) * !IN1 + !(IN0) * IN1) * !IN2 + !((IN0) * !IN1 + !(IN0) * IN1) * IN2) * IN3) * IN4) * IN5) * IN6) * IN7); PIN IN0 INV 1 999 1 0 1 0 PIN IN1 INV 1 999 1 0 1 0 PIN IN2 INV 1 999 1 0 1 0 PIN IN3 INV 1 999 1 0 1 0 PIN IN4 INV 1 999 1 0 1 0 PIN IN5 INV 1 999 1 0 1 0 PIN IN6 INV 1 999 1 0 1 0 PIN IN7 INV 1 999 1 0 1 0\nGATE BUFX1 5 Y=A; PIN A NONINV 1 999 1 0 1 0\nGATE ZERO 1 Y=CONST0;\nGATE CONE 1 Y=CONST1;\n";

void Target_data::set_signals() {
  signals.clear();
  signals.shrink_to_fit();
  if (inxcases.size() > 0) {
    for (auto node : inxcases[0]) {
      signals.push_back(node.first);
    }
  }
}

Target_data::Target_data(std::string target_name, std::vector<std::map<nodecircuit::Node *, bool> > target_inxcases) {
  name = target_name;
  inxcases = target_inxcases;
  set_signals();
}

Target_data::Target_data(std::string target_name, std::vector<std::map<nodecircuit::Node *, bool> > target_inxcases,
                         std::map<std::string, bool> target_constraints) {
  name = target_name;
  inxcases = target_inxcases;
  constraints = target_constraints;
  set_signals();
}

std::string Target_data::get_node_string() {
  std::string cubes;
  for (auto inxcase : inxcases) {
    std::string str_inxcase;
    for (auto signal : signals) {
      if (inxcase[signal]) str_inxcase += "1";
      else str_inxcase += "0";
    }
    cubes += str_inxcase + " 1\n";
  }
  std::string node;
  node += ".names";
  for (auto signal : signals) {
    node += ' ' + signal->name;
  }
  node += ' ' + name + '\n';
  node += cubes;

  return node;
}

int Target_data::get_node(ABC::Abc_Ntk_t *pNtk) {
  ABC::Abc_Obj_t *pNode, *pNet;
  pNode = ABC::Abc_NtkCreateNode(pNtk);
  //fanin
  for (auto signal : signals) {
    pNet = ABC::Abc_NtkFindOrCreateNet(pNtk, strdup((signal->name).c_str()));
    ABC::Abc_ObjAddFanin(pNode, pNet);
  }
  //fanout
  pNet = ABC::Abc_NtkFindOrCreateNet(pNtk, strdup(name.c_str()));
  ABC::Abc_ObjAddFanin(pNet, pNode);
  //function
  std::string cubes;
  if (inxcases.size() == 0) {
    cubes += " 0\n";
  }
  for (auto inxcase : inxcases) {
    std::string str_inxcase;
    for (auto signal : signals) {
      if (inxcase[signal]) str_inxcase += "1";
      else str_inxcase += "0";
    }
    cubes += str_inxcase + " 1\n";
  }
  ABC::Abc_ObjSetData(pNode, ABC::Abc_SopRegister((ABC::Mem_Flex_t *) pNtk->pManFunc, cubes.c_str()));

  return 0;
}

Adapter_data::Adapter_data(NodeVector &new_solution, NodeVector &initial_solution)
  : file_name("adapter_iwata.v") {
  // Quit when all nodes in new_solution contain in initial_solution.
  bool is_subset = true;
  for (auto &&node : new_solution) {
    if (std::find(initial_solution.begin(), initial_solution.end(), node) == initial_solution.end()) {
      is_subset = false;
      break;
    }
  }
  if (is_subset) {
    return;
  }

  NodeSet fanin_cone(new_solution.begin(), new_solution.end());
  for (auto &&node : initial_solution) {
    GetFaninCone(node, fanin_cone);
  }
  Circuit adapter;
  std::unordered_map<Node *, Node *> node_map;
  for (auto &&node : new_solution) {
    auto *new_node = new Node;
    adapter.inputs.push_back(new_node);
    *new_node = *node;
    new_node->is_input = true;
    new_node->is_output = false;
    adapter.all_nodes.push_back(new_node);
    adapter.all_nodes_map.insert(std::make_pair(new_node->name, new_node));
    node_map.insert(std::make_pair(node, new_node));
  }
  inputs = adapter.inputs;
  for (auto &&node : initial_solution) {
    Node *new_node;
    if (node_map.count(node) > 0) {
      new_node = node_map.at(node);
    } else {
      new_node = new Node;
      adapter.outputs.push_back(new_node);
      *new_node = *node;
      new_node->is_input = false;
    }
    new_node->is_output = true;
    adapter.all_nodes.push_back(new_node);
    adapter.all_nodes_map.insert(std::make_pair(new_node->name, new_node));
    node_map.insert(std::make_pair(node, new_node));
  }
  outputs = adapter.outputs;
  for (auto &&node : fanin_cone) {
    if (node_map.count(node) == 0) {
      Node *new_node;
      if (!node->is_input and !node->is_output) {
        new_node = node;
      } else {
        new_node = new Node;
        *new_node = *node;
        node_map.insert(std::make_pair(node, new_node));
        if (node->is_input) {
          new_node->is_input = false;
        }
        if (node->is_output) {
          new_node->is_output = false;
        }
      }
      adapter.all_nodes.push_back(new_node);
      adapter.all_nodes_map.insert(std::make_pair(new_node->name, new_node));
    }
  }

  for (auto &&node : adapter.all_nodes) {
    for (auto &&input : node->inputs) {
      if (node_map.count(input) > 0) {
        input = node_map.at(input);
      }
    }
    for (auto &&output : node->outputs) {
      if (node_map.count(output) > 0) {
        output = node_map.at(output);
      }
    }
  }
  //adapter.WriteVerilog(file_name + "_tmp");

  std::ifstream reading;
  reading.open(file_name + "_tmp", std::ios::in);
  std::ofstream writing;
  writing.open(file_name, std::ios::out);

  std::string str;
  getline(reading, str);
  str = "module " + file_name;
  writing << str;
  writing.seekp(-2, std::ios::cur);
  writing << " (" << std::endl;
  while (getline(reading, str)) {
    writing << str << std::endl;
  }
}

void Adapter_data::GetFaninCone(Node *node, NodeSet &fanin_nodes) {
  NodeVector not_processed;
  not_processed.push_back(node);
  while (!not_processed.empty()) {
    Node *current_node = not_processed.back();
    not_processed.pop_back();
    fanin_nodes.insert(current_node);

    for (auto &&fanin : current_node->inputs) {
      if (fanin_nodes.count(fanin) == 0) {
        not_processed.push_back(fanin);
      }
    }
  }
  fanin_nodes.erase(node);
}

Patch_data::Patch_data(std::vector<std::vector<Target_data> > target_groups) {
    std::cout << "begin constructing patch_data" << std::endl;
  adapter_data = NULL;
  std::string patch_name = "patch";
  pNtk = ABC::Abc_NtkStartRead(strdup(patch_name.c_str()));

  for (auto target_group : target_groups) {
    for (auto target_data : target_group) {
      outputs.push_back(target_data.name);
      for (auto signal : target_data.signals) {
        if ((signal->name)[0] != 't') {
          inputs.push_back(signal->name);
        }
      }
    }
  }

  std::sort(inputs.begin(), inputs.end());
  inputs.erase(std::unique(inputs.begin(), inputs.end()), inputs.end());
  std::sort(outputs.begin(), outputs.end());
  outputs.erase(std::unique(outputs.begin(), outputs.end()), outputs.end());

  for (auto input : inputs) {
    ABC::Io_ReadCreatePi(pNtk, strdup(input.c_str()));
  }
  for (auto output : outputs) {
    ABC::Io_ReadCreatePo(pNtk, strdup(output.c_str()));
  }

  for (auto target_group : target_groups) {
    //assert((target_group.size() > 0);
    if (target_group.size() == 1) {
      auto target_data = target_group[0];
      ////for blif
      std::string node = target_data.get_node_string();
      net.push_back(node);
      ////for abc
      target_data.get_node(pNtk);
    } else {
      std::vector<std::string> output_names;
      for (auto target_data : target_group) {
        output_names.push_back(target_data.name);
      }
      std::sort(output_names.begin(), output_names.end());
      output_names.erase(std::unique(output_names.begin(), output_names.end()), output_names.end());

      std::string id = output_names[0];
      int target_count = (int) output_names.size();
      int constraints_count = (int) pow(2, target_count);
      int intermediate_count = target_count * constraints_count;
      int c_count = (int) pow(2, target_count);

      std::vector<std::string> intermediate_names; //like "t_0_t0=0_t1=1"
      for (int i_i = 0; i_i < intermediate_count; i_i++) {
        std::string str_constraints;
        for (auto constraint : target_group[i_i].constraints) {
          str_constraints += constraint.first + '=';
          if (constraint.second) str_constraints += '1';
          else str_constraints += '0';
        }
        target_group[i_i].name += '_' + str_constraints; //(MEMO:if C???, id+C???)
        intermediate_names.push_back(target_group[i_i].name);
      }
      std::sort(intermediate_names.begin(), intermediate_names.end());


      std::vector<std::vector<std::string> > c_names_group(target_count + 1);//[num of 1][]

      for (int c_i = 0; c_i < c_count; c_i++) { //set C(c_i)
        std::string c_i_digital = "_";
        int c_i_tmp = c_i;
        int c_i_num_of_1 = 0;
        for (int t_i = 0; t_i < target_count; t_i++) {
          if (c_i_tmp % 2 == 0) {
            c_i_digital.insert(c_i_digital.begin(), '0');
          } else {
            c_i_digital.insert(c_i_digital.begin(), '1');
            c_i_num_of_1 += 1;
          }
          c_i_tmp /= 2;
        }

        std::string c_name = 'c' + c_i_digital + id;
        c_names_group[c_i_num_of_1].push_back(c_name);

        std::string cube;
        for (int t_i = 0; t_i < target_count; t_i++) {//set cube by t_i(any constraints)
          //std::string c_i_partial = c_i_digital.substr(0,t_i) + c_i_digital.substr(t_i+1,target_count-t_i-1);
          //int c_i_partial_dec = 0;
          //for(int c_i_partial_i = 0; c_i_partial_i < target_count-1; c_i_partial_i++) {
          //  if(c_i_partial[c_i_partial_i] == '1') c_i_partial_dec += (int)pow(2,target_count-2-c_i_partial_i);
          //}
          for (int co_i = 0; co_i < constraints_count; co_i++) {
            if (co_i == c_i/*c_i_partial_dec*/) cube += '1'/*c_i_digital[t_i]*/;
            else cube += '-';
          }
        }
        cube += " 1\n";

        ////for blif
        std::string node;
        node += ".names";
        for (auto input : intermediate_names) {
          node += ' ' + input;
        }
        node += ' ' + c_name + '\n';
        node += cube;
        net.push_back(node);

        ////for abc
        ABC::Abc_Obj_t *pNode, *pNet;
        pNode = ABC::Abc_NtkCreateNode(pNtk);
        //fanin
        for (auto input : intermediate_names) {
          pNet = ABC::Abc_NtkFindOrCreateNet(pNtk, strdup(input.c_str()));
          ABC::Abc_ObjAddFanin(pNode, pNet);
        }
        //fanout
        pNet = ABC::Abc_NtkFindOrCreateNet(pNtk, strdup(c_name.c_str()));
        ABC::Abc_ObjAddFanin(pNet, pNode);
        //function
        ABC::Abc_ObjSetData(pNode, ABC::Abc_SopRegister((ABC::Mem_Flex_t *) pNtk->pManFunc, cube.c_str()));
      }

      for (int t_i = 0; t_i < target_count; t_i++) {// set t_i
        std::string cubes;
        std::map<std::string, char> inxcase; // {c???, 0or1or-}
        for (int num_of_1 = 0; num_of_1 < target_count; num_of_1++) {// inxcase is all don't care at first.
          for (auto c_name : c_names_group[num_of_1]) {
            inxcase[c_name] = '-';
          }
        }
        for (int num_of_1 = target_count; num_of_1 > 0; num_of_1--) {
          for (auto c_name : c_names_group[num_of_1]) {//set from c11...1, then c11...10s
            if (c_name[t_i + 1] == '1') { //[t_i+1] because of "cxx...x"
              inxcase[c_name] = '1';
              for (auto item : inxcase) {
                cubes += item.second;
              }
              cubes += " 1\n";
              inxcase[c_name] = '-';
            } else {
              inxcase[c_name] = '0';
            }
          }
          //	    for(auto c_name : c_names_group[num_of_1]) {//set 0 in inxcase
          //	      if(c_name[t_i+1] == '0') {
          //		inxcase[c_name] = '0';
          //	      }
          //	    }
        }
        ////for blif
        std::string node;
        node += ".names";
        for (auto item : inxcase) {
          node += ' ' + item.first;
        }
        node += ' ' + output_names[t_i] + '\n';
        node += cubes;
        net.push_back(node);

        ////for abc
        ABC::Abc_Obj_t *pNode, *pNet;
        pNode = ABC::Abc_NtkCreateNode(pNtk);
        //fanin
        for (auto item : inxcase) {
          pNet = ABC::Abc_NtkFindOrCreateNet(pNtk, strdup((item.first).c_str()));
          ABC::Abc_ObjAddFanin(pNode, pNet);
        }
        //fanout
        pNet = ABC::Abc_NtkFindOrCreateNet(pNtk, strdup(output_names[t_i].c_str()));
        ABC::Abc_ObjAddFanin(pNet, pNode);
        //function
        ABC::Abc_ObjSetData(pNode, ABC::Abc_SopRegister((ABC::Mem_Flex_t *) pNtk->pManFunc, cubes.c_str()));
      }

      for (auto target_data : target_group) {
        ////for blif
        std::string node = target_data.get_node_string();
        net.push_back(node);
        ////for abc
        target_data.get_node(pNtk);
      }
    }
  }
  ABC::Abc_Ntk_t *pTemp;
  pAbc = ABC::Abc_FrameGetGlobalFrame();
  ABC::Abc_FrameReplaceCurrentNetwork(pAbc, pNtk);
  ABC::Abc_NtkFinalizeRead(pNtk);
  if (ABC::Abc_NtkCheckRead(pNtk) == 0) {
    std::cout << pNtk << "Circuit is wrong" << std::endl;
    //assert((0);                 
  }
  pNtk = ABC::Abc_NtkToLogic(pTemp = pNtk);
  ABC::Abc_FrameReplaceCurrentNetwork(pAbc, pNtk); ABC::Abc_FrameClearVerifStatus(pAbc);
  
  std::cout << "patch_data has been constructed" << std::endl;
}

void Patch_data::generate_blif(std::string filename) {
  std::cout << "begin generating patch blif" << std::endl;
  std::ofstream writing_blif;
  writing_blif.open(filename, std::ios::out);

  writing_blif << ".model patch\n.inputs";
  for (auto input : inputs) {
    writing_blif << ' ' << input;
  }
  writing_blif << "\n.outputs";
  for (auto output : outputs) {
    writing_blif << ' ' << output;
  }
  writing_blif << '\n';
  for (auto node : net) {
    writing_blif << node;
  }
  writing_blif << ".end\n";
  std::cout << "patch blif has been generated" << std::endl;
}


void Patch_data::generate_verilog(std::string filename) {
  std::cout << "begin generating patch verilog" << std::endl;
  if (adapter_data == NULL) {
    std::string Command = "strash;dc2;dc2;map;write_verilog " + filename;
    if (ABC::Cmd_CommandExecute(pAbc, strdup(Command.c_str()))) {
      std::cout << "Command execution error: " << Command << std::endl;
      exit(1);
    }
  }
  else {
    std::string Command1 = "strash;dc2;dc2;write_verilog tmp_patch_sub.v";
    if (ABC::Cmd_CommandExecute(pAbc, strdup(Command1.c_str()))) {
      std::cout << "Command execution error: " << Command1 << std::endl;
      exit(1);
    }
    
    std::ofstream writing_top;
    writing_top.open("tmp_patch_top.v", std::ios::out);
    
    writing_top << "module patch (\n";
    for (auto input_node : adapter_data->inputs) {
      writing_top << ' ' << input_node->name << ',';
    }
    writing_top << '\n';
    for (auto output : outputs) {
      writing_top << ' ' << output << ',';
    }
    writing_top.seekp(-1, std::ios::cur);
    writing_top << ");\n";
    
    writing_top << "input";
    for (auto input_node : adapter_data->inputs) {
      writing_top << ' ' << input_node->name << ',';
    }
    writing_top.seekp(-1, std::ios::cur);
    writing_top << ";\n";
    writing_top << "output";
    for (auto output : outputs) {
      writing_top << ' ' << output << ',';
    }
    writing_top.seekp(-1, std::ios::cur);
    writing_top << ";\n";
    writing_top << "wire";
    for (auto wire_node : adapter_data->outputs) {
      writing_top << ' ' << wire_node->name << ',';
    }
    writing_top.seekp(-1, std::ios::cur);
    writing_top << ";\n";
    
    writing_top << "tmp_patch_sub patch_sub(";
    for (auto output : outputs) {
      writing_top << '.' << output << '(' << output << "),";
    }
    for (auto input : inputs) {
      writing_top << '.' << input << '(' << input << "),";
    }
    writing_top.seekp(-1, std::ios::cur);
    writing_top << ");" << std::endl;
    
    writing_top << adapter_data->file_name;
    writing_top.seekp(-2, std::ios::cur);
    writing_top <<" adapter0(";
    for (auto output_node : adapter_data->outputs) {
      writing_top << '.' << output_node->name << '(' << output_node->name << "),";
    }
    for (auto input_node : adapter_data->inputs) {
      writing_top << '.' << input_node->name << '(' << input_node->name << "),";
    }
    writing_top.seekp(-1, std::ios::cur);
    writing_top << ");" << std::endl;
    writing_top << "endmodule" << std::endl;
    
    std::ifstream reading_patch_sub;
    reading_patch_sub.open("tmp_patch_sub.v", std::ios::in);
    std::string str;
    while (getline(reading_patch_sub, str)) {
      if(str[0] == 'm') break;
    }
    writing_top << "module tmp_patch_sub (" << std::endl;
    while (getline(reading_patch_sub, str)) {
      writing_top << str << std::endl;	
    }
    std::ifstream reading_adapter;
    reading_adapter.open(adapter_data->file_name, std::ios::in);
    while (getline(reading_adapter, str)) {
      writing_top << str << std::endl;	
    }
    
    std::string Command2 = "read_verilog tmp_patch_top.v;strash;dc2;dc2;map;write_verilog " + filename;
    if (ABC::Cmd_CommandExecute(pAbc, strdup(Command2.c_str()))) {
      std::cout << "Command execution error: " << Command2 << std::endl;
      exit(1);
    }
  }
  std::cout << "patch verilog has been generated" << std::endl;
}

void patchgen::generate_out(std::string file_impl, std::string file_out, Patch_data patch_data) {
  std::cout << "begin generating out verilog" << std::endl;
  std::ifstream reading_impl;
  reading_impl.open(file_impl, std::ios::in);
  std::ofstream writing_out;
  writing_out.open(file_out, std::ios::out);

  std::string str;
  while (getline(reading_impl, str)) {
    if (str == "endmodule") {
      writing_out << "patch p0(";
      for (auto output : patch_data.outputs) {
        writing_out << '.' << output << '(' << output << "),";
      }
      if (patch_data.adapter_data != NULL) {
       for (auto input_node : patch_data.adapter_data->inputs) {
         writing_out << '.' << input_node->name << '(' << input_node->name << "),";
       }
      } else {
        for (auto input : patch_data.inputs) {
          writing_out << '.' << input << '(' << input << "),";
        }
      }
      writing_out.seekp(-1, std::ios::cur);
      writing_out << ");" << std::endl;

    }
    writing_out << str << std::endl;
  }
  std::cout << "out verilog has been generated" << std::endl;
}

void patchgen::read_library()
{
  ABC::Mio_Library_t *pLib = ABC::Mio_LibraryRead("lib", (char*)lib, NULL, 0);
  ABC::Mio_UpdateGenlib(pLib);
}

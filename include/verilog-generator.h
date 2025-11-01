#ifndef VERILOG_GENERATOR_H
#define VERILOG_GENERATOR_H
#include <string>
#include "implicant.h"

// ASKANDRANI
using std::string;

class VerilogGenerator {
  VerilogGenerator(vector<Implicant>&);
  void build(string &filename);
};
#endif //VERILOG_GENERATOR_H

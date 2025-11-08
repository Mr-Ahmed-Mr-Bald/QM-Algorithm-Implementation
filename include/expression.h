#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>
using std::vector;
using std::string;

class Expression {

  // TODO: Handle initialization properly
  // Member variables
  public:
  int numberOfBits; // Number of bits
  vector<int> minterms, dontcares; // Lists of minterms and don't cares

  // Member functions
  public:
  Expression() = default;
  void read(); // Deprecated - throws error
  bool read_from_file(const string& filename); // Read from file in required format
  bool evalute(const vector<int>&);
};

#endif //EXPRESSION_H

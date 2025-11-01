#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
using std::vector;

// AMONIOS
class Expression {

  // TODO: Handle initialization properly
  // Member variables
  public:
  int numberOfBits; // Number of bits
  vector<int> minterms, dontcares; // Lists of minterms and don't cares

  // Member functions
  public:
  Expression() = default;
  void read();
  bool evalute(const vector<int>&);
};

#endif //EXPRESSION_H

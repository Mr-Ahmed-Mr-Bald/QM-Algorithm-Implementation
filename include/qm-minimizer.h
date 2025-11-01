#ifndef QM_MINIMIZER_H
#define QM_MINIMIZER_H

#include <string>
#include "expression.h"
#include "implicant.h"

using std::string;

class QMMinimizer {
  // Member variables
  bool initialized = false;
  vector<vector<Implicant>> implicant_groups;

  // Constructor
  QMMinimizer() = default;
  QMMinimizer(const Expression&);

  // Member functions
  void initialize(const Expression&);
  // combine ASKANDRANI
  bool combine(vector<vector<Implicant>>&, vector<vector<Implicant>>&);
  // bitmask BALD
  // minimize
  void minimize(vector<Implicant>&, vector<bool>&, vector<int>&, vector<vector<Implicant>>&); // AMONIOS
};

#endif // QM_MINIMIZER_H
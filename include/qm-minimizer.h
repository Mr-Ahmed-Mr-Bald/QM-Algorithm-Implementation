#ifndef QM_MINIMIZER_H
#define QM_MINIMIZER_H

// Standard Library Includes //
#include <string>

// Project Includes //
#include "expression.h"
#include "implicant.h"

// Namespace Usage //
using std::string;

class QMMinimizer {
  // Member variables //
  int numberOfBits;
  vector<vector<Implicant>> implicant_groups;

  // Constructors //
  
  /*
  Note: No default constructor is provided as an Implicant must be initialized with a list of bits.
  */

  public:
  QMMinimizer(const Expression&);

  // Member functions
  // combine ASKANDRANI
  bool combine(vector<vector<Implicant>>&, vector<vector<Implicant>>&);
  // bitmask BALD
  // minimize
  // [PIs], [EPIs], [EPIs coverage], [Minimzed expressions as list of implicants]
  void minimize(vector<Implicant>&, vector<bool>&, vector<int>&, vector<vector<Implicant>>&); // AMONIOS

  // Destructor //
  ~QMMinimizer() = default;
};

#endif // QM_MINIMIZER_H
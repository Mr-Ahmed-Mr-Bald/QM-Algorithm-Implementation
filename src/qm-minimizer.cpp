#include "../include/qm-minimizer.h"

// Constructors
QMMinimizer::QMMinimizer(const Expression &expression) {

  // Initialize member variables
  numberOfBits = expression.numberOfBits;

  implicant_groups.resize(numberOfBits + 1);

  for(int minterm : expression.minterms) {

    // Get group index based on number of ones in minterm
    int group_idx = __builtin_popcount(minterm);
    
    // Ensure valid group index
    assert(group_idx >= 0 && group_idx <= numberOfBits);

    // Create implicant and add to group
    implicant_groups[group_idx].emplace_back(Implicant(minterm, numberOfBits));
  }
}
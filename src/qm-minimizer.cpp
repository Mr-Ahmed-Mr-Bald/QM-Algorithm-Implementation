#include "../include/qm-minimizer.h"
#include "qm-minimizer.h"

// Constructors
QMMinimizer::QMMinimizer(const Expression &expression) {

  // Set expression
  this->expression = expression;

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

  for(int dontcare : expression.dontcares) {

    // Get group index based on number of ones in dontcare
    int group_idx = __builtin_popcount(dontcare);
    
    // Ensure valid group index
    assert(group_idx >= 0 && group_idx <= numberOfBits);

    // Create implicant and add to group
    implicant_groups[group_idx].emplace_back(Implicant(dontcare, numberOfBits));
  }

}

/*
Note: 
This is a heuristic method; it does not actually implement Petrick's method. I will change
that for the fully functional version.
For now, it returns one valid solution, but not necessarily the minimal one.
*/
void QMMinimizer::petrick(const vector<Implicant> &pe, vector<bool>& epi, vector<vector<int>>& solutions) {
  set<int> to_be_covered(expression.minterms.begin(), expression.minterms.end());
  solutions.resize(1);
  int m = pe.size();
  for(int i = 0; i < m; i++) {
    if (epi[i]) {
      const auto& covered_terms = pe[i].get_covered_terms();
      for(int term : covered_terms) {
        if (to_be_covered.find(term) != to_be_covered.end()) {
          to_be_covered.erase(term);
        }
      }
      solutions[0].push_back(i);
    }
  }
  for(int term : to_be_covered) {
    for(int i = 0; i < m; i++) {
      const auto& covered_terms = pe[i].get_covered_terms();
      if (std::find(covered_terms.begin(), covered_terms.end(), term) != covered_terms.end()) {
        solutions[0].push_back(i);
        break;
      }
    }
  }

  
}
#include "../include/qm-minimizer.h"
#include "qm-minimizer.h"
#include <algorithm>
#include <set>

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

// Helper function to combine two adjacent groups
void combine_helper(const vector<Implicant> &group1, const vector<Implicant> &group2, 
                   vector<Implicant> &combined, vector<bool> &used1, vector<bool> &used2) {
  for(size_t i = 0; i < group1.size(); i++) {
    for(size_t j = 0; j < group2.size(); j++) {
      // Check if they differ by exactly one bit
      if (group1[i] - group2[j] == 1) {
        // Combine them
        Implicant new_implicant = group1[i] + group2[j];
        
        // Check if this implicant already exists in combined
        bool exists = false;
        for(const auto &existing : combined) {
          if (existing == new_implicant) {
            exists = true;
            break;
          }
        }
        
        if (!exists) {
          combined.push_back(new_implicant);
        }
        
        // Mark as used
        used1[i] = true;
        used2[j] = true;
      }
    }
  }
}

// Combine adjacent groups and collect prime implicants
bool QMMinimizer::combine(vector<vector<Implicant>> &current_groups, 
                         vector<vector<Implicant>> &next_groups) {
  int num_groups = current_groups.size();
  if (num_groups <= 1) {
    return false; // Nothing to combine
  }
  
  next_groups.clear();
  next_groups.resize(num_groups - 1);
  
  bool any_combined = false;
  
  for(int i = 0; i < num_groups - 1; i++) {
    const auto &group1 = current_groups[i];
    const auto &group2 = current_groups[i + 1];
    
    vector<Implicant> combined;
    vector<bool> used1(group1.size(), false);
    vector<bool> used2(group2.size(), false);
    
    combine_helper(group1, group2, combined, used1, used2);
    
    if (!combined.empty()) {
      any_combined = true;
    }
    
    next_groups[i] = combined;
  }
  
  return any_combined;
}

void QMMinimizer::minimize(vector<Implicant> &pe, vector<bool> &epi, 
                          vector<int> &epi_coverage, 
                          vector<vector<Implicant>> &minimized_expressions) {
  // Step 1: Generate all prime implicants through iterative combination
  vector<vector<Implicant>> current_groups = implicant_groups;
  vector<vector<Implicant>> next_groups;
  
  // Track which implicants at each level were successfully combined
  set<Implicant> all_primes;
  
  while(true) {
    next_groups.clear();
    next_groups.resize(current_groups.size() > 0 ? current_groups.size() - 1 : 0);
    
    // Track which implicants were combined in this iteration
    vector<set<int>> used_in_groups(current_groups.size());
    
    bool any_combined = false;
    
    // Try to combine adjacent groups
    for(size_t i = 0; i < current_groups.size() - 1; i++) {
      const auto &group1 = current_groups[i];
      const auto &group2 = current_groups[i + 1];
      
      for(size_t j = 0; j < group1.size(); j++) {
        for(size_t k = 0; k < group2.size(); k++) {
          // Check if they differ by exactly one bit
          if (group1[j] - group2[k] == 1) {
            // Combine them
            Implicant new_implicant = group1[j] + group2[k];
            
            // Check if this implicant already exists in next_groups[i]
            bool exists = false;
            for(const auto &existing : next_groups[i]) {
              if (existing == new_implicant) {
                exists = true;
                break;
              }
            }
            
            if (!exists) {
              next_groups[i].push_back(new_implicant);
            }
            
            // Mark these implicants as used
            used_in_groups[i].insert(j);
            used_in_groups[i + 1].insert(k);
            any_combined = true;
          }
        }
      }
    }
    
    // Collect prime implicants (unused ones from current iteration)
    for(size_t i = 0; i < current_groups.size(); i++) {
      for(size_t j = 0; j < current_groups[i].size(); j++) {
        // If this implicant was NOT used in combination, it's a prime
        if (used_in_groups[i].find(j) == used_in_groups[i].end()) {
          all_primes.insert(current_groups[i][j]);
        }
      }
    }
    
    if (!any_combined) {
      break; // No more combinations possible
    }
    
    current_groups = next_groups;
  }
  
  // Convert set to vector
  pe.clear();
  for(const auto &impl : all_primes) {
    pe.push_back(impl);
  }
  
  // Step 2: Identify essential prime implicants
  epi.resize(pe.size(), false);
  epi_coverage.clear();
  
  // Build coverage table - for each minterm, find which PIs cover it
  for(int minterm : expression.minterms) {
    vector<int> covering_pis;
    
    for(size_t i = 0; i < pe.size(); i++) {
      auto covered = pe[i].get_covered_terms();
      if (std::find(covered.begin(), covered.end(), minterm) != covered.end()) {
        covering_pis.push_back(i);
      }
    }
    
    // If only one PI covers this minterm, it's essential
    if (covering_pis.size() == 1) {
      epi[covering_pis[0]] = true;
    }
  }
  
  // Step 3: Use Petrick's method to find minimal covering
  vector<vector<int>> solutions;
  petrick(pe, epi, solutions);
  
  // Convert solutions to implicant lists
  minimized_expressions.clear();
  for(const auto &sol : solutions) {
    vector<Implicant> expr;
    for(int idx : sol) {
      if (idx >= 0 && idx < static_cast<int>(pe.size())) {
        expr.push_back(pe[idx]);
      }
    }
    minimized_expressions.push_back(expr);
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
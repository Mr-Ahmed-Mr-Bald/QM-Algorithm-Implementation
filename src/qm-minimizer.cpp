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
                          vector<vector<Implicant>> &minimized_expressions,
                          vector<vector<int>> &solutions_indices) {
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

  // Return raw solution indices to caller
  solutions_indices = solutions;

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

// Petrick's method to find minimal covering of remaining minterms
void QMMinimizer::petrick(const vector<Implicant> &pe, vector<bool>& epi, vector<vector<int>>& solutions) {

  // Identify minterms that still need to be covered
  set<int> to_be_covered(expression.minterms.begin(), expression.minterms.end());
  for(int i = 0; i < int(pe.size()); i++) {
    if (epi[i]) {
      const auto& covered_terms = pe[i].get_covered_terms();
      for(int term : covered_terms) {
        if (to_be_covered.find(term) != to_be_covered.end()) {
          to_be_covered.erase(term);
        }
      }
    }
  }

  if (to_be_covered.empty()) {
    solutions.emplace_back();
    for (int i = 0; i < int(pe.size()); i++) {
      if (epi[i]) {
        solutions[0].push_back(i);
      }
    }
    return;
  }

  // Build the Petrick's method table
  int remaining = int(to_be_covered.size());
  vector<vector<set<int>>> P(remaining);
  for(int i = 0; i < int(pe.size()); i++) {
    if (epi[i]) continue;
    const auto& covered_terms = pe[i].get_covered_terms();
    for(int term : covered_terms) {
      auto it = to_be_covered.find(term);
      if (it != to_be_covered.end()) {
        P[distance(to_be_covered.begin(), it)].push_back({i});
      }
    }
  }

  // Minimize product of sums
  while(int(P.size()) > 1) {
    vector<vector<set<int>>> new_P;
    for(int i = 0; i < int(P.size()); i += 2) {
      assert(!P[i].empty());
      if (i + 1 < int(P.size())) {
        assert(!P[i + 1].empty());
        // Get the common set of implicants between P[i] and P[i + 1]
        // (X + Y)(X + Z) = X + YZ
        vector<set<int>> common;
        for(const auto &a : P[i]) {
          for(const auto &b : P[i + 1]) {
            if (a == b) common.push_back(a);
          }
        }

        auto remove_common = [&](vector<set<int>> &from) {
          from.erase(
            remove_if(
              from.begin(), from.end(),
              [&](const set<int> &s) {
                return find(common.begin(), common.end(), s) != common.end();
              }
            ), from.end()
          );
        };

        // Remove common implicants from both P[i] and P[i + 1]
        remove_common(P[i]);
        remove_common(P[i + 1]);

        // Multiply remaining implicants
        auto multiplied = multiply(P[i], P[i + 1]);
        // Merge common and multiplied
        common.insert(common.end(), multiplied.begin(), multiplied.end());

        // Remove duplicates
        sort(common.begin(), common.end(),
          [](const set<int> &s1, const set<int> &s2) {
            return s1.size() < s2.size();
          });

        common.erase(
          unique(common.begin(), common.end()),
          common.end()
        );

        new_P.push_back(common);
      } else {
        new_P.push_back(P[i]);
      }
    }

    P = new_P;
  }

  // Conver into sum of products
  while(int(P.size()) > 1) {
    auto multiplied = multiply(P[int(P.size()) - 2], P[int(P.size()) - 1]);
    P.pop_back();
    P.pop_back();
    P.push_back(multiplied);
  }

  // Find the term with the least number of implicants
  int min_size = INT_MAX;
  for(const auto &s : P[0]) {
    if (int(s.size()) < min_size) {
      min_size = int(s.size());
    }
  }

  for(const auto &s : P[0]) {
    if (int(s.size()) == min_size) {
      vector<int> solution(s.begin(), s.end());
      for(int i = 0; i < int(pe.size()); i++) {
        if (epi[i]) {
          solution.push_back(i);
        }
      }
      solutions.push_back(solution);
    }
  }
}



vector<set<int>> QMMinimizer::multiply(const vector<set<int>> &a, const vector<set<int>> &b) {
  vector<set<int>> product;

  for(const auto &set_a : a) {
    for(const auto &set_b : b) {
      set<int> combined = set_a;
      combined.insert(set_b.begin(), set_b.end());
      product.push_back(combined);
    }
  }

  sort(product.begin(), product.end(),
    [](const set<int> &s1, const set<int> &s2) {
      return s1.size() < s2.size();
    });

  // Remove supersets
  vector<set<int>> minimized;
  for(const auto &s : product) {
    bool is_superset = false;
    for(const auto &m : minimized) {
      if (includes(s.begin(), s.end(), m.begin(), m.end())) {
        is_superset = true;
        break;
      }
    }

    if (!is_superset) {
      minimized.push_back(s);
    }
  }

  return minimized;
}

// Select minimal-cost solutions from a set of candidate solutions
// Cost metric: n-input AND or OR gate cost = 2*n + 2
// NOT gates are not counted as they are considered negligible
// We compute per-solution cost as: sum(AND_cost for each implicant) + OR_cost(for combining implicants if more than one)
void QMMinimizer::select_min_cost_solutions(const vector<Implicant> &pe, const vector<vector<int>> &solutions, vector<vector<int>> &out_min_solutions) {
  out_min_solutions.clear();
  if (solutions.empty()) return;

  vector<long long> costs;
  costs.reserve(solutions.size());

  for (const auto &sol : solutions) {
    long long total = 0;
    int m = static_cast<int>(sol.size());

    // Sum cost of each implicant (as an AND of literals)
    for (int idx : sol) {
      if (idx < 0 || idx >= static_cast<int>(pe.size())) continue;
      auto prod = pe[idx].generate_product();
      int n_literals = static_cast<int>(prod.size());

      // AND gate cost for this product: 2*n + 2 (where n = number of inputs)
      int and_cost = (n_literals > 0) ? (2 * n_literals + 2) : 0;

      total += and_cost;
    }

    // OR gate to combine product terms (if more than one product)
    if (m > 1) {
      int or_cost = 2 * m + 2;
      total += or_cost;
    }

    costs.push_back(total);
  }

  // Find minimum cost
  long long min_cost = LLONG_MAX;
  for (auto c : costs) if (c < min_cost) min_cost = c;

  // Collect solutions with minimal cost
  for (size_t i = 0; i < solutions.size(); ++i) {
    if (costs[i] == min_cost) {
      out_min_solutions.push_back(solutions[i]);
    }
  }
}

  
  
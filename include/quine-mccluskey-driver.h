#ifndef QUINE_MCCLUSKEY_DRIVER_H
#define QUINE_MCCLUSKEY_DRIVER_H

#include <string>
#include <vector>
#include "expression.h"
#include "implicant.h"

using std::string;
using std::vector;

class QuineMcCluskeyDriver {
private:
    Expression expression;
    vector<Implicant> prime_implicants;
    vector<bool> essential_pis;
    vector<int> epi_coverage;
    vector<int> uncovered_minterms;
    vector<vector<Implicant>> minimized_expressions;
    vector<vector<int>> solution_indices;
    vector<vector<int>> minimal_cost_solution_indices;
    vector<vector<Implicant>> minimal_cost_expressions;
    
    bool expression_loaded;
    bool minimization_done;
    
    void calculate_uncovered_minterms();

public:
    QuineMcCluskeyDriver();
    
    // File-based input (as per project requirements)
    bool load_from_file(const string& filename);
    
    // Interactive input (alternative)
    void read_expression_interactive();
    
    // Core workflow
    void run_minimization();
    
    // Display methods (as per project requirements)
    void display_prime_implicants() const;        // Requirement 2
    void display_essential_pis() const;            // Requirement 3
    void display_minimized_expressions() const;    // Requirement 4
    void display_min_cost_expressions() const; // Display minimal-cost solutions
    void display_all_results() const;              // Combined display
    
    // Verilog generation (bonus - requirement 5)
    void generate_verilog(const string& filename = "");
    
    // Utility
    bool is_expression_loaded() const { return expression_loaded; }
    bool is_minimization_done() const { return minimization_done; }
    void reset();
    
    // Interactive menu
    void run_interactive();
    void run_batch(const string& input_file, const string& output_file = "");
};

#endif // QUINE_MCCLUSKEY_DRIVER_H
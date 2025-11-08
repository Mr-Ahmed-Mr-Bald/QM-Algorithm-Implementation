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
    vector<vector<Implicant>> minimized_expressions;
    vector<vector<int>> solution_indices;
    
    bool expression_loaded;
    bool minimization_done;

public:
    QuineMcCluskeyDriver();
    
    // Core workflow methods
    void read_expression();
    void run_minimization();
    void display_results() const;
    void generate_verilog(const string& filename = "");
    
    // Utility methods
    bool is_expression_loaded() const { return expression_loaded; }
    bool is_minimization_done() const { return minimization_done; }
    void reset();
    
    // Interactive menu
    void run_interactive();
};

#endif // QUINE_MCCLUSKEY_DRIVER_H
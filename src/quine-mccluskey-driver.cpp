#include "../include/quine-mccluskey-driver.h"
#include "../include/qm-minimizer.h"
#include "../include/verilog-generator.h"
#include "../include/file-parser.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <set>
#include <limits>

using namespace std;

QuineMcCluskeyDriver::QuineMcCluskeyDriver() 
    : expression_loaded(false), minimization_done(false) {}

bool QuineMcCluskeyDriver::load_from_file(const string& filename) {
    cout << "Reading input file: " << filename << "\n";
    
    if (!FileParser::parse_file(filename, expression)) {
        return false;
    }
    
    expression_loaded = true;
    minimization_done = false;
    
    cout << " File loaded successfully!\n";
    cout << "  Number of variables: " << expression.numberOfBits << "\n";
    cout << "  Minterms: ";
    for(size_t i = 0; i < expression.minterms.size(); i++) {
        cout << expression.minterms[i];
        if (i < expression.minterms.size() - 1) cout << ", ";
    }
    cout << "\n  Don't cares: ";
    if (expression.dontcares.empty()) {
        cout << "none";
    } else {
        for(size_t i = 0; i < expression.dontcares.size(); i++) {
            cout << expression.dontcares[i];
            if (i < expression.dontcares.size() - 1) cout << ", ";
        }
    }
    cout << "\n";
    
    return true;
}

void QuineMcCluskeyDriver::read_expression_interactive() {
    expression.read();
    expression_loaded = true;
    minimization_done = false;
}

void QuineMcCluskeyDriver::run_minimization() {
    if (!expression_loaded) {
        cout << "✗ Error: No expression loaded!\n";
        return;
    }

    cout << "\n--- Running Quine-McCluskey Minimization ---\n";
    
    QMMinimizer qm(expression);
    
    prime_implicants.clear();
    essential_pis.clear();
    epi_coverage.clear();
    minimized_expressions.clear();
    solution_indices.clear();
    
    // Get minimized expressions and raw solution indices (from Petrick)
    qm.minimize(prime_implicants, essential_pis, epi_coverage, minimized_expressions, solution_indices);

    // Also compute minimal-cost solutions (based on literal count)
    minimal_cost_solution_indices.clear();
    minimal_cost_expressions.clear();
    qm.select_min_cost_solutions(prime_implicants, solution_indices, minimal_cost_solution_indices);

    // Build minimal_cost_expressions mapping indices -> Implicant objects
    for(const auto &sol : minimal_cost_solution_indices) {
        vector<Implicant> expr;
        for(int idx : sol) {
            if (idx >= 0 && idx < static_cast<int>(prime_implicants.size())) {
                expr.push_back(prime_implicants[idx]);
            }
        }
        minimal_cost_expressions.push_back(expr);
    }
    
    calculate_uncovered_minterms();
    
    minimization_done = true;
    cout << "Minimization completed!\n";
}

void QuineMcCluskeyDriver::calculate_uncovered_minterms() {
    uncovered_minterms.clear();
    set<int> covered;
    
    for(size_t i = 0; i < prime_implicants.size(); i++) {
        if (essential_pis[i]) {
            auto terms = prime_implicants[i].get_covered_terms();
            for(int t : terms) {
                covered.insert(t);
            }
        }
    }
    
    for(int m : expression.minterms) {
        if (covered.find(m) == covered.end()) {
            uncovered_minterms.push_back(m);
        }
    }
}

// Requirement 2: Generate and print all prime implicants
void QuineMcCluskeyDriver::display_prime_implicants() const {
    if (!minimization_done) {
        cout << "Error: Run minimization first!\n";
        return;
    }
    
    cout << "\n" << string(70, '=') << "\n";
    cout << "2. PRIME IMPLICANTS\n";
    cout << string(70, '=') << "\n";
    cout << left << setw(5) << "PI#" 
         << setw(15) << "Binary" 
         << setw(20) << "Algebraic"
         << "Covers Minterms\n";
    cout << string(70, '-') << "\n";
    
    for(size_t i = 0; i < prime_implicants.size(); i++) {
        cout << setw(5) << i;
        
        // Binary representation
        string binary;
        for(int j = 0; j < prime_implicants[i].get_number_of_bits(); j++) {
            ImplicantBit bit = prime_implicants[i].get_bit(j);
            if (bit == ImplicantBit::$zero) binary += "0";
            else if (bit == ImplicantBit::$one) binary += "1";
            else binary += "-";
        }
        cout << setw(15) << binary;
        
        // Algebraic form
        auto product = prime_implicants[i].generate_product();
        string algebra;
        if (product.empty()) {
            algebra = "1";
        } else {
            for(size_t j = 0; j < product.size(); j++) {
                char var = 'A' + product[j].first;
                algebra += var;
                if (product[j].second) algebra += "'";
            }
        }
        cout << setw(20) << algebra;
        
        // Covered terms
        vector<int> terms = prime_implicants[i].get_covered_terms();
        cout << "{";
        for(size_t j = 0; j < terms.size(); j++) {
            cout << terms[j];
            if (j < terms.size() - 1) cout << ", ";
        }
        cout << "}";
        
        if (essential_pis[i]) cout << " [ESSENTIAL]";
        cout << "\n";
    }
    cout << string(70, '=') << "\n";
}

// Requirement 3: Essential PIs and uncovered minterms
void QuineMcCluskeyDriver::display_essential_pis() const {
    if (!minimization_done) {
        cout << "✗ Error: Run minimization first!\n";
        return;
    }
    
    cout << "\n" << string(70, '=') << "\n";
    cout << "3. ESSENTIAL PRIME IMPLICANTS\n";
    cout << string(70, '=') << "\n";
    
    bool found = false;
    for(size_t i = 0; i < prime_implicants.size(); i++) {
        if (essential_pis[i]) {
            found = true;
            auto product = prime_implicants[i].generate_product();
            if (product.empty()) {
                cout << "   1";
            } else {
                cout << "   ";
                for(size_t j = 0; j < product.size(); j++) {
                    char var = 'A' + product[j].first;
                    cout << var;
                    if (product[j].second) cout << "'";
                }
            }
            cout << "\n";
        }
    }
    
    if (!found) {
        cout << "   No essential prime implicants\n";
    }
    
    cout << "\nMinterms not covered by EPIs: ";
    if (uncovered_minterms.empty()) {
        cout << "None (all covered)\n";
    } else {
        cout << "{";
        for(size_t i = 0; i < uncovered_minterms.size(); i++) {
            cout << uncovered_minterms[i];
            if (i < uncovered_minterms.size() - 1) cout << ", ";
        }
        cout << "}\n";
    }
    cout << string(70, '=') << "\n";
}

// Requirement 4: Print minimized Boolean expression
void QuineMcCluskeyDriver::display_minimized_expressions() const {
    if (!minimization_done) {
        cout << "Error: Run minimization first!\n";
        return;
    }
    
    cout << "\n" << string(70, '=') << "\n";
    cout << "4. MINIMIZED BOOLEAN EXPRESSIONS\n";
    cout << string(70, '=') << "\n";
    
    if (minimized_expressions.empty()) {
        cout << "   F = 0 (no valid solutions)\n";
    } else {
        for(size_t sol = 0; sol < minimized_expressions.size(); sol++) {
            cout << "Solution " << (sol + 1) << ": F = ";
            
            if (minimized_expressions[sol].empty()) {
                cout << "0";
            } else {
                for(size_t i = 0; i < minimized_expressions[sol].size(); i++) {
                    auto product = minimized_expressions[sol][i].generate_product();
                    if (product.empty()) {
                        cout << "1";
                    } else {
                        for(size_t j = 0; j < product.size(); j++) {
                            char var = 'A' + product[j].first;
                            cout << var;
                            if (product[j].second) cout << "'";
                        }
                    }
                    if (i < minimized_expressions[sol].size() - 1) cout << " + ";
                }
            }
            cout << "\n";
        }
    }
    cout << string(70, '=') << "\n";
}

void QuineMcCluskeyDriver::display_min_cost_expressions() const {
    if (!minimization_done) {
        cout << "Error: Run minimization first!\n";
        return;
    }

    cout << "\n" << string(70, '=') << "\n";
    cout << "4b. MINIMAL-COST MINIMIZED EXPRESSIONS (by literal count)" << "\n";
    cout << string(70, '=') << "\n";

    if (minimal_cost_expressions.empty()) {
        cout << "   (no minimal-cost solutions computed)\n";
    } else {
        for(size_t sol = 0; sol < minimal_cost_expressions.size(); sol++) {
            cout << "Solution " << (sol + 1) << ": F = ";

            if (minimal_cost_expressions[sol].empty()) {
                cout << "0";
            } else {
                for(size_t i = 0; i < minimal_cost_expressions[sol].size(); i++) {
                    auto product = minimal_cost_expressions[sol][i].generate_product();
                    if (product.empty()) {
                        cout << "1";
                    } else {
                        for(size_t j = 0; j < product.size(); j++) {
                            char var = 'A' + product[j].first;
                            cout << var;
                            if (product[j].second) cout << "'";
                        }
                    }
                    if (i < minimal_cost_expressions[sol].size() - 1) cout << " + ";
                }
            }
            cout << "\n";
        }
    }

    cout << string(70, '=') << "\n";
}

void QuineMcCluskeyDriver::display_all_results() const {
    display_prime_implicants();
    display_essential_pis();
    display_minimized_expressions();
    display_min_cost_expressions();
}

void QuineMcCluskeyDriver::generate_verilog(const string& filename) {
    if (!minimization_done) {
        cout << "Error: Run minimization first!\n";
        return;
    }

    const vector<vector<int>> &vgen_solutions = minimal_cost_solution_indices.empty() ? solution_indices : minimal_cost_solution_indices;
    VerilogGenerator vgen(expression, prime_implicants, vgen_solutions);
    
    // Ask user for output style
    cout << "\nSelect Verilog output style:\n";
    cout << "1. Assign statement (dataflow)\n";
    cout << "2. Always block (behavioral)\n";
    cout << "3. Case statement\n";
    cout << "4. Primitive gates (and, or, not) - PROJECT REQUIREMENT\n";
    cout << "Enter choice (1-4, default 4): ";
    
    int style_choice = 4;  // Default to primitives
    string input_line;
    getline(cin, input_line);
    
    if (!input_line.empty()) {
        try {
            style_choice = stoi(input_line);
        } catch (...) {
            style_choice = 4;
        }
    }
    
    switch(style_choice) {
        case 1:
            vgen.set_output_style(VerilogGenerator::OutputStyle::Assign);
            break;
        case 2:
            vgen.set_output_style(VerilogGenerator::OutputStyle::Always);
            break;
        case 3:
            vgen.set_output_style(VerilogGenerator::OutputStyle::Case);
            break;
        case 4:
        default:
            vgen.set_output_style(VerilogGenerator::OutputStyle::Primitives);
            break;
    }
    
    string verilog_code = vgen.render_verilog();
    cout << "\n" << string(70, '=') << "\n";
    cout << "5. VERILOG MODULE (BONUS)\n";
    cout << string(70, '=') << "\n";
    cout << verilog_code;
    cout << string(70, '=') << "\n";
    
    if (!filename.empty()) {
        ofstream outfile(filename);
        if (outfile.is_open()) {
            vgen.write_to_file(outfile);
            outfile.close();
            cout << "Verilog saved to: " << filename << "\n";
        } else {
            cout << "Error: Could not write to file: " << filename << "\n";
        }
    }
}

void QuineMcCluskeyDriver::reset() {
    expression_loaded = false;
    minimization_done = false;
    prime_implicants.clear();
    essential_pis.clear();
    epi_coverage.clear();
    minimized_expressions.clear();
    solution_indices.clear();
}

void QuineMcCluskeyDriver::run_batch(const string& input_file, const string& output_file) {
    cout << "\n=== BATCH MODE ===\n";
    
    if (!load_from_file(input_file)) {
        return;
    }
    
    run_minimization();
    display_all_results();
    
    if (!output_file.empty()) {
        generate_verilog(output_file);
    }
}

void QuineMcCluskeyDriver::run_interactive() {
    cout << "\n";
    cout << "=====Quine-McCluskey Boolean Minimizer=====\n";
    cout << "\n";

    int choice;
    while(true) {
        cout << "\n========== Main Menu ==========\n";
        cout << "1. Load expression from file\n";
        cout << "2. Enter expression manually\n";
        cout << "3. Run minimization\n";
        cout << "4. Display results\n";
        cout << "5. Generate Verilog\n";
        cout << "6. Exit\n";
        cout << "===============================\n";
        cout << "Choice: ";
        cin >> choice;
        
        // Clear input buffer after reading choice
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch(choice) {
            case 1: {
                string filename;
                cout << "Enter filename: ";
                getline(cin, filename);
                if (!filename.empty()) {
                    load_from_file(filename);
                } else {
                    cout << "Error: Filename cannot be empty!\n";
                }
                break;
            }
            case 2:
                read_expression_interactive();
                break;
            case 3:
                run_minimization();
                break;
            case 4:
                display_all_results();
                break;
            case 5: {
                string filename;
                cout << "Enter output filename (or press Enter to display only): ";
                getline(cin, filename);
                generate_verilog(filename);
                break;
            }
            case 6:
                cout << "\nThank you!\n";
                return;
            default:
                cout << "Invalid choice!\n";
        }
    }
}

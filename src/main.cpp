#include "expression.h"
#include "qm-minimizer.h"
#include "verilog-generator.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

void print_header() {
    cout << "╔════════════════════════════════════════════════╗\n";
    cout << "║  Quine-McCluskey Logic Minimization Tool      ║\n";
    cout << "║  CSCE2301 - Digital Design I - Fall 2025      ║\n";
    cout << "╚════════════════════════════════════════════════╝\n\n";
}

void print_usage() {
    cout << "Usage: QM_Algorithm_Implementation <input_file> [output_file]\n";
    cout << "  input_file  : Text file with 3 lines (variables, minterms/maxterms, don't-cares)\n";
    cout << "  output_file : Optional Verilog output file (default: output.v)\n\n";
    cout << "Input file format:\n";
    cout << "  Line 1: Number of variables\n";
    cout << "  Line 2: Minterms (m0,m1,...) or Maxterms (M0,M1,...)\n";
    cout << "  Line 3: Don't cares (d0,d1,...) or empty\n\n";
    cout << "Example:\n";
    cout << "  3\n";
    cout << "  m1,m3,m6,m7\n";
    cout << "  d0,d5\n";
}

int main(int argc, char* argv[]) {
    print_header();
    
    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    string input_file = argv[1];
    string output_file = (argc >= 3) ? argv[2] : "output.v";
    
    // Step 1: Read input
    cout << "Reading input from: " << input_file << "\n";
    cout << "==========================================\n";
    
    Expression expr;
    if (!expr.read_from_file(input_file)) {
        cerr << "Error: Failed to read input file\n";
        return 1;
    }
    
    cout << "\n✓ Input validated successfully\n";
    cout << "  Variables: " << expr.numberOfBits << "\n";
    cout << "  Minterms: ";
    for (int m : expr.minterms) cout << m << " ";
    cout << "\n  Don't cares: ";
    for (int d : expr.dontcares) cout << d << " ";
    cout << "\n\n";
    
    // Step 2: Run Quine-McCluskey minimization
    cout << "Running Quine-McCluskey Minimization...\n";
    cout << "==========================================\n";
    
    QMMinimizer qm(expr);
    vector<Implicant> prime_implicants;
    vector<bool> essential_pis;
    vector<int> epi_coverage;
    vector<vector<Implicant>> minimized_expressions;
    
    qm.minimize(prime_implicants, essential_pis, epi_coverage, minimized_expressions);
    
    cout << "✓ Minimization completed\n\n";
    
    // Step 3: Print Prime Implicants
    cout << "Prime Implicants (PIs):\n";
    cout << "==========================================\n";
    for (size_t i = 0; i < prime_implicants.size(); i++) {
        cout << "PI" << i << ": ";
        
        // Binary representation
        for (int j = 0; j < prime_implicants[i].get_number_of_bits(); j++) {
            ImplicantBit bit = prime_implicants[i].get_bit(j);
            if (bit == ImplicantBit::$zero) cout << "0";
            else if (bit == ImplicantBit::$one) cout << "1";
            else cout << "-";
        }
        
        // Covered terms
        vector<int> terms = prime_implicants[i].get_covered_terms();
        cout << " | Covers: {";
        for (size_t j = 0; j < terms.size(); j++) {
            cout << terms[j];
            if (j < terms.size() - 1) cout << ", ";
        }
        cout << "}";
        
        if (essential_pis[i]) cout << " [ESSENTIAL]";
        cout << "\n";
    }
    cout << "\n";
    
    // Step 4: Print Essential Prime Implicants
    cout << "Essential Prime Implicants (EPIs):\n";
    cout << "==========================================\n";
    vector<int> covered_by_epis;
    for (size_t i = 0; i < prime_implicants.size(); i++) {
        if (essential_pis[i]) {
            auto product = prime_implicants[i].generate_product();
            if (product.empty()) {
                cout << "1";
            } else {
                for (size_t j = 0; j < product.size(); j++) {
                    if (product[j].second) cout << "x" << product[j].first << "'";
                    else cout << "x" << product[j].first;
                }
            }
            cout << "\n";
            
            // Track covered minterms
            vector<int> terms = prime_implicants[i].get_covered_terms();
            covered_by_epis.insert(covered_by_epis.end(), terms.begin(), terms.end());
        }
    }
    
    // Remove duplicates and sort
    sort(covered_by_epis.begin(), covered_by_epis.end());
    covered_by_epis.erase(unique(covered_by_epis.begin(), covered_by_epis.end()), covered_by_epis.end());
    
    // Find uncovered minterms
    vector<int> uncovered;
    for (int m : expr.minterms) {
        if (find(covered_by_epis.begin(), covered_by_epis.end(), m) == covered_by_epis.end()) {
            uncovered.push_back(m);
        }
    }
    
    cout << "\nMinterms NOT covered by EPIs: {";
    for (size_t i = 0; i < uncovered.size(); i++) {
        cout << uncovered[i];
        if (i < uncovered.size() - 1) cout << ", ";
    }
    cout << "}\n\n";
    
    // Step 5: Print Minimized Expressions
    cout << "Minimized Boolean Expression(s):\n";
    cout << "==========================================\n";
    for (size_t sol = 0; sol < minimized_expressions.size(); sol++) {
        cout << "Solution " << (sol + 1) << ": ";
        bool first = true;
        for (const auto &impl : minimized_expressions[sol]) {
            if (!first) cout << " + ";
            first = false;
            
            auto product = impl.generate_product();
            if (product.empty()) {
                cout << "1";
            } else {
                for (size_t j = 0; j < product.size(); j++) {
                    if (product[j].second) cout << "x" << product[j].first << "'";
                    else cout << "x" << product[j].first;
                }
            }
        }
        cout << "\n";
    }
    cout << "\n";
    
    // Step 6: Generate Verilog module
    cout << "Generating Verilog Module...\n";
    cout << "==========================================\n";
    
    // Build solution indices
    vector<vector<int>> solution_indices;
    for (const auto &min_expr : minimized_expressions) {
        vector<int> indices;
        for (const auto &impl : min_expr) {
            for (size_t i = 0; i < prime_implicants.size(); i++) {
                if (prime_implicants[i] == impl) {
                    indices.push_back(i);
                    break;
                }
            }
        }
        solution_indices.push_back(indices);
    }
    
    VerilogGenerator vgen(expr, prime_implicants, solution_indices);
    vgen.set_module_name("minimized_module");
    vgen.set_output_name("f");
    vgen.set_output_style(VerilogGenerator::OutputStyle::Assign);
    
    string verilog_code = vgen.render_verilog();
    cout << verilog_code << "\n";
    
    // Save to file
    ofstream outfile(output_file);
    if (outfile.is_open()) {
        vgen.write_to_file(outfile);
        outfile.close();
        cout << "✓ Verilog module saved to: " << output_file << "\n";
    } else {
        cerr << "✗ Error: Could not write to output file\n";
    }
    
    cout << "\n==========================================\n";
    cout << "✓ Processing completed successfully!\n";
    
    return 0;
}
#include "../include/quine-mccluskey-driver.h"
#include "../include/qm-minimizer.h"
#include "../include/verilog-generator.h"
#include <iostream>
#include <fstream>

using namespace std;

QuineMcCluskeyDriver::QuineMcCluskeyDriver() 
    : expression_loaded(false), minimization_done(false) {}

void QuineMcCluskeyDriver::read_expression() {
    cout << "\n--- Enter Boolean Expression ---\n";
    expression.read();
    expression_loaded = true;
    minimization_done = false;
    
    cout << "\n✓ Expression entered successfully!\n";
    cout << "Number of bits: " << expression.numberOfBits << "\n";
    cout << "Minterms: ";
    for(int m : expression.minterms) cout << m << " ";
    cout << "\nDon't cares: ";
    for(int d : expression.dontcares) cout << d << " ";
    cout << "\n";
}

void QuineMcCluskeyDriver::run_minimization() {
    if (!expression_loaded) {
        cout << "\n✗ Error: Please enter an expression first!\n";
        return;
    }

    cout << "\n--- Running Quine-McCluskey Minimization ---\n";
    
    QMMinimizer qm(expression);
    
    prime_implicants.clear();
    essential_pis.clear();
    epi_coverage.clear();
    minimized_expressions.clear();
    solution_indices.clear();
    
    qm.minimize(prime_implicants, essential_pis, epi_coverage, minimized_expressions);
    
    // Build solution indices for Verilog generator
    for(const auto &min_expr : minimized_expressions) {
        vector<int> indices;
        for(const auto &impl : min_expr) {
            for(size_t i = 0; i < prime_implicants.size(); i++) {
                if (prime_implicants[i] == impl) {
                    indices.push_back(i);
                    break;
                }
            }
        }
        solution_indices.push_back(indices);
    }
    
    minimization_done = true;
    cout << "\n✓ Minimization completed!\n";
    cout << "Found " << prime_implicants.size() << " prime implicants\n";
    cout << "Found " << minimized_expressions.size() << " minimal solution(s)\n";
}

void QuineMcCluskeyDriver::display_results() const {
    if (!minimization_done) {
        cout << "\n✗ Error: Please run minimization first!\n";
        return;
    }

    cout << "\n========== MINIMIZATION RESULTS ==========\n\n";
    
    // Display prime implicants
    cout << "Prime Implicants:\n";
    for(size_t i = 0; i < prime_implicants.size(); i++) {
        cout << "PI" << i << ": ";
        
        // Print implicant bits
        for(int j = 0; j < prime_implicants[i].get_number_of_bits(); j++) {
            ImplicantBit bit = prime_implicants[i].get_bit(j);
            if (bit == ImplicantBit::$zero) cout << "0";
            else if (bit == ImplicantBit::$one) cout << "1";
            else cout << "-";
        }
        
        // Print covered terms
        vector<int> terms = prime_implicants[i].get_covered_terms();
        cout << " (";
        for(size_t j = 0; j < terms.size(); j++) {
            cout << terms[j];
            if (j < terms.size() - 1) cout << ", ";
        }
        cout << ")";
        
        if (essential_pis[i]) cout << " [ESSENTIAL]";
        cout << "\n";
    }
    
    // Display minimized expressions
    cout << "\nMinimized Expression(s):\n";
    for(size_t sol = 0; sol < minimized_expressions.size(); sol++) {
        cout << "Solution " << (sol + 1) << ": ";
        bool first = true;
        for(const auto &impl : minimized_expressions[sol]) {
            if (!first) cout << " + ";
            first = false;
            
            auto product = impl.generate_product();
            if (product.empty()) {
                cout << "1";
            } else {
                for(size_t j = 0; j < product.size(); j++) {
                    if (product[j].second) cout << "x" << product[j].first << "'";
                    else cout << "x" << product[j].first;
                }
            }
        }
        cout << "\n";
    }
    cout << "==========================================\n";
}

void QuineMcCluskeyDriver::generate_verilog(const string& filename) {
    if (!minimization_done) {
        cout << "\n✗ Error: Please run minimization first!\n";
        return;
    }

    cout << "\n--- Verilog Generation ---\n";
    
    string module_name, output_name;
    int style_choice;
    
    cout << "Enter module name (default: minimized_module): ";
    cin.ignore();
    getline(cin, module_name);
    if (module_name.empty()) module_name = "minimized_module";
    
    cout << "Enter output name (default: f): ";
    getline(cin, output_name);
    if (output_name.empty()) output_name = "f";
    
    cout << "\nSelect output style:\n";
    cout << "1. Assign statement\n";
    cout << "2. Always block\n";
    cout << "3. Case statement\n";
    cout << "Choice: ";
    cin >> style_choice;
    
    VerilogGenerator vgen(expression, prime_implicants, solution_indices);
    vgen.set_module_name(module_name);
    vgen.set_output_name(output_name);
    
    switch(style_choice) {
        case 1: vgen.set_output_style(VerilogGenerator::OutputStyle::Assign); break;
        case 2: vgen.set_output_style(VerilogGenerator::OutputStyle::Always); break;
        case 3: vgen.set_output_style(VerilogGenerator::OutputStyle::Case); break;
        default: vgen.set_output_style(VerilogGenerator::OutputStyle::Assign);
    }
    
    string verilog_code = vgen.render_verilog();
    cout << "\n========== Generated Verilog Module ==========\n";
    cout << verilog_code;
    cout << "==============================================\n";
    
    // Save to file
    string save_filename = filename;
    if (save_filename.empty()) {
        char save_choice;
        cout << "\nSave to file? (y/n): ";
        cin >> save_choice;
        
        if (save_choice == 'y' || save_choice == 'Y') {
            cout << "Enter filename (e.g., module.v): ";
            cin >> save_filename;
        }
    }
    
    if (!save_filename.empty()) {
        ofstream outfile(save_filename);
        if (outfile.is_open()) {
            vgen.write_to_file(outfile);
            outfile.close();
            cout << "✓ Verilog module saved to " << save_filename << "\n";
        } else {
            cout << "✗ Error: Could not open file for writing\n";
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

void QuineMcCluskeyDriver::run_interactive() {
    cout << "╔════════════════════════════════════════════════╗\n";
    cout << "║     Quine-McCluskey Boolean Minimizer         ║\n";
    cout << "║     Digital Logic Design Project              ║\n";
    cout << "╚════════════════════════════════════════════════╝\n";

    int choice;
    while(true) {
        cout << "\n========== Quine-McCluskey Minimizer ==========\n";
        cout << "1. Enter Boolean Expression\n";
        cout << "2. Run Minimization\n";
        cout << "3. Display Results\n";
        cout << "4. Generate Verilog Module\n";
        cout << "5. Reset\n";
        cout << "6. Exit\n";
        cout << "===============================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1: read_expression(); break;
            case 2: run_minimization(); break;
            case 3: display_results(); break;
            case 4: generate_verilog(); break;
            case 5: 
                reset();
                cout << "\n✓ System reset. Ready for new expression.\n";
                break;
            case 6:
                cout << "\nThank you for using QM Minimizer!\n";
                return;
            default:
                cout << "\n✗ Invalid choice! Please try again.\n";
        }
    }
}

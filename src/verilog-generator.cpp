#include "../include/verilog-generator.h"
#include <sstream>
#include <algorithm>
#include <cassert>

using std::stringstream;

// Private member to store output style
namespace {
  VerilogGenerator::OutputStyle current_style = VerilogGenerator::OutputStyle::Assign;
  vector<string> custom_input_names;
}

// Constructor
VerilogGenerator::VerilogGenerator(const Expression &Ex, const vector<Implicant> &_pe, 
                                const vector<vector<int>> &_solutions)
    : main_ex(Ex), pe(_pe), solutions(_solutions) {
  // Validate that we have valid data
  assert(main_ex.numberOfBits > 0);
}

// Configuration methods
void VerilogGenerator::set_module_name(const string &mname) {
  module_name = VerilogUtils::escape_identifier(mname);
}

void VerilogGenerator::set_output_name(const string &oname) {
  output_name = VerilogUtils::escape_identifier(oname);
}

void VerilogGenerator::set_input_prefix(const string &IP) {
  input_prefix = IP;
  custom_input_names.clear(); // Clear custom names when prefix changes
}

void VerilogGenerator::set_input_names(const vector<string> &names) {
  assert(static_cast<int>(names.size()) == main_ex.numberOfBits);
  custom_input_names = names;
  // Escape all identifiers
  for(auto &name : custom_input_names) {
    name = VerilogUtils::escape_identifier(name);
  }
}

void VerilogGenerator::set_output_style(OutputStyle style) {
  current_style = style;
}

// Info methods
int VerilogGenerator::get_number_of_inputs() const {
  return main_ex.numberOfBits;
}

int VerilogGenerator::get_number_of_solutions() const {
  return static_cast<int>(solutions.size());
}

// Helper to get input names
vector<string> get_input_names_list(int numBits, const string &prefix) {
  if (!custom_input_names.empty()) {
    return custom_input_names;
  }
  
  vector<string> names;
  for(int i = 0; i < numBits; i++) {
    names.push_back(VerilogUtils::escape_identifier(prefix + std::to_string(i)));
  }
  return names;
}

// Convert an implicant to a Verilog product term
string VerilogGenerator::implicant_to_verilog_expr(const Implicant &imp, 
                                                   const vector<string> &input_names) {
  auto product = imp.generate_product();
  
  if (product.empty()) {
    return "1'b1"; // Tautology (all dashes)
  }
  
  stringstream ss;
  bool first = true;
  
  for(const auto &term : product) {
    if (!first) {
      ss << " & ";
    }
    first = false;
    
    int var_idx = term.first;
    bool is_negated = term.second;
    
    if (is_negated) {
      ss << "~";
    }
    ss << input_names[var_idx];
  }
  
  return ss.str();
}

// Render a solution expression (SOP form)
string VerilogGenerator::render_solution_expression(int index) const {
  if (index < 0 || index >= static_cast<int>(solutions.size())) {
    return "";
  }
  
  const auto &solution = solutions[index];
  
  if (solution.empty()) {
    return "1'b0"; // Empty solution means always false
  }
  
  vector<string> input_names = get_input_names_list(main_ex.numberOfBits, input_prefix);
  
  stringstream ss;
  bool first = true;
  
  for(int impl_idx : solution) {
    if (impl_idx < 0 || impl_idx >= static_cast<int>(pe.size())) {
      continue; // Skip invalid indices
    }
    
    if (!first) {
      ss << " | ";
    }
    first = false;
    
    string product = implicant_to_verilog_expr(pe[impl_idx], input_names);
    
    // Add parentheses if product contains multiple terms
    if (product.find('&') != string::npos) {
      ss << "(" << product << ")";
    } else {
      ss << product;
    }
  }
  
  return ss.str();
}

// Render all solutions as comments
string VerilogGenerator::render_all_solutions_comments() {
  stringstream ss;
  
  ss << "// Alternative minimized solutions:\n";
  
  for(int i = 0; i < static_cast<int>(solutions.size()); i++) {
    ss << "// Solution " << (i + 1) << ": ";
    ss << render_solution_expression(i) << "\n";
  }
  
  return ss.str();
}

// Main Verilog module rendering
string VerilogGenerator::render_verilog() {
  stringstream ss;
  
  vector<string> input_names = get_input_names_list(main_ex.numberOfBits, input_prefix);
  string escaped_output = VerilogUtils::escape_identifier(output_name);
  string escaped_module = VerilogUtils::escape_identifier(module_name);
  
  // Module declaration
  ss << "module " << escaped_module << " (\n";
  
  // Input declarations
  ss << "    input ";
  for(int i = 0; i < main_ex.numberOfBits; i++) {
    ss << input_names[i];
    if (i < main_ex.numberOfBits - 1) {
      ss << ", ";
    }
  }
  ss << ",\n";
  
  // Output declaration
  ss << "    output ";
  if (current_style == OutputStyle::Always) {
    ss << "reg ";
  }
  ss << escaped_output << "\n";
  ss << ");\n\n";
  
  // Add solution comments if multiple solutions exist
  if (solutions.size() > 1) {
    ss << render_all_solutions_comments() << "\n";
  }
  
  // Generate logic based on style
  if (solutions.empty() || solutions[0].empty()) {
    // No solution - output always 0
    if (current_style == OutputStyle::Always) {
      ss << "    always @(*) begin\n";
      ss << "        " << escaped_output << " = 1'b0;\n";
      ss << "    end\n";
    } else {
      ss << "    assign " << escaped_output << " = 1'b0;\n";
    }
  } else {
    string expr = render_solution_expression(0);
    
    switch(current_style) {
      case OutputStyle::Assign:
        ss << "    assign " << escaped_output << " = " << expr << ";\n";
        break;
        
      case OutputStyle::Always:
        ss << "    always @(*) begin\n";
        ss << "        " << escaped_output << " = " << expr << ";\n";
        ss << "    end\n";
        break;
        
      case OutputStyle::Case: {
        // Case style - build case statement
        ss << "    always @(*) begin\n";
        ss << "        case ({";
        for(int i = 0; i < main_ex.numberOfBits; i++) {
          ss << input_names[i];
          if (i < main_ex.numberOfBits - 1) ss << ", ";
        }
        ss << "})\n";
        
        // Generate case items for each minterm
        for(int minterm : main_ex.minterms) {
          ss << "            " << main_ex.numberOfBits << "'b";
          for(int i = main_ex.numberOfBits - 1; i >= 0; i--) {
            ss << ((minterm >> i) & 1);
          }
          ss << ": " << escaped_output << " = 1'b1;\n";
        }
        
        ss << "            default: " << escaped_output << " = 1'b0;\n";
        ss << "        endcase\n";
        ss << "    end\n";
        break;
      }
    }
  }
  
  ss << "\nendmodule\n";
  
  return ss.str();
}

// Write to file
void VerilogGenerator::write_to_file(ofstream &outfile) {
  if (!outfile.is_open()) {
    throw std::runtime_error("Output file is not open");
  }
  
  string verilog_code = render_verilog();
  outfile << verilog_code;
}
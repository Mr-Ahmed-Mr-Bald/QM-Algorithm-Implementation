#include "../include/verilog-generator.h"
#include <sstream>
#include <algorithm>
#include <cassert>

using std::stringstream;

// Constructor
VerilogGenerator::VerilogGenerator(const Expression &Ex, const vector<Implicant> &_pe, 
                                const vector<vector<int>> &_solutions)
    : main_ex(Ex), pe(_pe), solutions(_solutions), 
      output_style(OutputStyle::Always) {
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
  output_style = style;
}

// Info methods
int VerilogGenerator::get_number_of_inputs() const {
  return main_ex.numberOfBits;
}

int VerilogGenerator::get_number_of_solutions() const {
  return static_cast<int>(solutions.size());
}

// Helper to get input names
vector<string> VerilogGenerator::get_input_names_list() const {
  if (!custom_input_names.empty()) {
    return custom_input_names;
  }
  
  vector<string> names;
  for(int i = 0; i < main_ex.numberOfBits; i++) {
    names.push_back(VerilogUtils::escape_identifier(input_prefix + std::to_string(i)));
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
  
  vector<string> input_names = get_input_names_list();
  
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
  // Delegate to appropriate style-specific function
  switch(output_style) {
    case OutputStyle::Assign:
      return render_verilog_assign();
    case OutputStyle::Always:
      return render_verilog_always();
    case OutputStyle::Case:
      return render_verilog_case();
    case OutputStyle::Primitives:
      return render_verilog_primitives();
    default:
      return render_verilog_assign();
  }
}

// Render Verilog using assign statement
string VerilogGenerator::render_verilog_assign() {
  stringstream ss;
  
  vector<string> input_names = get_input_names_list();
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
  ss << "    output " << escaped_output << "\n";
  ss << ");\n\n";
  
  // Add solution comments if multiple solutions exist
  if (solutions.size() > 1) {
    ss << render_all_solutions_comments() << "\n";
  }
  
  // Generate assign statement
  if (solutions.empty() || solutions[0].empty()) {
    ss << "    assign " << escaped_output << " = 1'b0;\n";
  } else {
    string expr = render_solution_expression(0);
    ss << "    assign " << escaped_output << " = " << expr << ";\n";
  }
  
  ss << "\nendmodule\n";
  
  return ss.str();
}

// Render Verilog using always block
string VerilogGenerator::render_verilog_always() {
  stringstream ss;
  
  vector<string> input_names = get_input_names_list();
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
  ss << "    output reg " << escaped_output << "\n";
  ss << ");\n\n";
  
  // Add solution comments if multiple solutions exist
  if (solutions.size() > 1) {
    ss << render_all_solutions_comments() << "\n";
  }
  
  // Generate always block
  ss << "    always @(*) begin\n";
  if (solutions.empty() || solutions[0].empty()) {
    ss << "        " << escaped_output << " = 1'b0;\n";
  } else {
    string expr = render_solution_expression(0);
    ss << "        " << escaped_output << " = " << expr << ";\n";
  }
  ss << "    end\n";
  
  ss << "\nendmodule\n";
  
  return ss.str();
}

// Render Verilog using case statement
string VerilogGenerator::render_verilog_case() {
  stringstream ss;
  
  vector<string> input_names = get_input_names_list();
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
  ss << "    output reg " << escaped_output << "\n";
  ss << ");\n\n";
  
  // Add solution comments if multiple solutions exist
  if (solutions.size() > 1) {
    ss << render_all_solutions_comments() << "\n";
  }
  
  // Generate case statement
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
  
  ss << "\nendmodule\n";
  
  return ss.str();
}

// Render Verilog using primitive gates (and, or, not)
string VerilogGenerator::render_verilog_primitives() {
  stringstream ss;
  
  vector<string> input_names = get_input_names_list();
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
  ss << "    output " << escaped_output << "\n";
  ss << ");\n\n";
  
  // Add solution comments if multiple solutions exist
  if (solutions.size() > 1) {
    ss << render_all_solutions_comments() << "\n";
  }
  
  // Handle empty solution
  if (solutions.empty() || solutions[0].empty()) {
    ss << "    // No minterms - output always 0\n";
    ss << "    assign " << escaped_output << " = 1'b0;\n";
    ss << "\nendmodule\n";
    return ss.str();
  }
  
  const auto &solution = solutions[0];
  
  // Wire declarations for internal signals
  vector<string> product_wires;  // Wires for each product term (AND gates)
  vector<string> not_wires;      // Wires for NOT gates
  
  // First, identify which variables need to be inverted
  vector<bool> need_not(main_ex.numberOfBits, false);
  for(int impl_idx : solution) {
    if (impl_idx < 0 || impl_idx >= static_cast<int>(pe.size())) continue;
    
    auto product = pe[impl_idx].generate_product();
    for(const auto &term : product) {
      if (term.second) {  // negated
        need_not[term.first] = true;
      }
    }
  }
  
  // Declare wires for inverted signals
  ss << "    // Internal wires\n";
  for(int i = 0; i < main_ex.numberOfBits; i++) {
    if (need_not[i]) {
      string not_wire = input_names[i] + "_n";
      not_wires.push_back(not_wire);
      ss << "    wire " << not_wire << ";\n";
    } else {
      not_wires.push_back("");
    }
  }
  
  // Declare wires for product terms
  for(size_t i = 0; i < solution.size(); i++) {
    string prod_wire = "product" + std::to_string(i);
    product_wires.push_back(prod_wire);
    ss << "    wire " << prod_wire << ";\n";
  }
  ss << "\n";
  
  // Generate NOT gates
  ss << "    // NOT gates for inverted inputs\n";
  for(int i = 0; i < main_ex.numberOfBits; i++) {
    if (need_not[i]) {
      ss << "    not(" << not_wires[i] << ", " << input_names[i] << ");\n";
    }
  }
  if (std::any_of(need_not.begin(), need_not.end(), [](bool b){ return b; })) {
    ss << "\n";
  }
  
  // Generate AND gates for each product term
  ss << "    // AND gates for product terms\n";
  for(size_t i = 0; i < solution.size(); i++) {
    int impl_idx = solution[i];
    if (impl_idx < 0 || impl_idx >= static_cast<int>(pe.size())) continue;
    
    auto product = pe[impl_idx].generate_product();
    
    if (product.empty()) {
      // Tautology - product term is always 1
      ss << "    assign " << product_wires[i] << " = 1'b1;\n";
    } else if (product.size() == 1) {
      // Single literal - direct connection
      int var_idx = product[0].first;
      bool is_negated = product[0].second;
      
      if (is_negated) {
        ss << "    assign " << product_wires[i] << " = " << not_wires[var_idx] << ";\n";
      } else {
        ss << "    assign " << product_wires[i] << " = " << input_names[var_idx] << ";\n";
      }
    } else {
      // Multiple literals - AND gate
      ss << "    and(" << product_wires[i];
      for(const auto &term : product) {
        int var_idx = term.first;
        bool is_negated = term.second;
        
        if (is_negated) {
          ss << ", " << not_wires[var_idx];
        } else {
          ss << ", " << input_names[var_idx];
        }
      }
      ss << ");\n";
    }
  }
  ss << "\n";
  
  // Generate OR gate for sum of products
  ss << "    // OR gate for sum of products\n";
  if (product_wires.size() == 1) {
    // Single product term - direct connection
    ss << "    assign " << escaped_output << " = " << product_wires[0] << ";\n";
  } else {
    // Multiple product terms - OR gate
    ss << "    or(" << escaped_output;
    for(const auto &wire : product_wires) {
      ss << ", " << wire;
    }
    ss << ");\n";
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
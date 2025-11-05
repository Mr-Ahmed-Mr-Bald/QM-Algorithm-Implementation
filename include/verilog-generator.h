#ifndef VERILOG_GENERATOR_H
#define VERILOG_GENERATOR_H

// --- Standard library includes ---
#include <string>
#include <fstream>
#include <vector>

// --- Project includes ---
#include "implicant.h"
#include "expression.h"

// --- Helper utilities ---
#include "../utils/verilog_utils.h"

// Use some standard namespace stuff to keep code cleaner
using std::string;
using std::ofstream;
using std::vector;

class VerilogGenerator {
  // === Member variables ===

  // Stores the original boolean expression (main info like input count, etc.)
  Expression main_ex;

  // List of all prime implicants found by the minimizer
  vector<Implicant> pe;

  // Each entry in this vector is a valid cover (solution),
  // stored as indices pointing to the implicants above.
  vector<vector<int>> solutions;

  // Default names for module, inputs, and outputs (can be changed later)
  string module_name = "minimized_module";
  string output_name = "f";
  string input_prefix = "x";

public:
  // Different styles for how we want the Verilog output to look
  enum class OutputStyle {
    Assign,   // Basic style: "assign f = <expr>;"
    Always,   // Uses always block: "always @(*) f = <expr>;"
    Case      // Case-based style (good for wide outputs)
  };

  // === Constructors ===

  // No default constructor – we need an expression and implicants to work with
  VerilogGenerator(const Expression &Ex, const vector<Implicant> &_pe, const vector<vector<int>> &_solutions);

  // === Configuration methods ===
  // Let user change names for module, output, and input prefix
  void set_module_name(const string &mname);
  void set_output_name(const string &oname);
  void set_input_prefix(const string &IP);

  // Use custom input names instead of the default prefix naming
  void set_input_names(const vector<string> &names);

  // Choose which output style we want (assign, always, or case)
  void set_output_style(OutputStyle style);

  // === Info / Query methods ===
  int get_number_of_inputs() const;
  int get_number_of_solutions() const;

  // === Main rendering methods ===

  // Create a full Verilog module as a string (based on current settings)
  string render_verilog();

  // Write the generated Verilog module straight to a file
  void write_to_file(ofstream &outfile);

  // Create the right-hand side (RHS) of an assign statement
  // for a specific minimized solution
  string render_solution_expression(int index) const;

  // Same as above but wraps it with "assign <output> = ..."
  string render_solution_assign(int index) const;

  // Show all minimized solutions as commented-out alternatives
  // (so the user can see all possible covers)
  string render_all_solutions_comments();

  // === Helper / Utility stuff ===

  // Turns an implicant into a valid Verilog expression
  // (e.g., A'B + BC' => (~A & B) | (B & ~C))
  static string implicant_to_verilog_expr(const Implicant &imp, const vector<string> &input_names);
};

// Inline convenience wrapper: formats an `assign` statement using the RHS
// produced by `render_solution_expression`. Kept inline as it's trivial and
// avoids an extra symbol in the .cpp.
inline string VerilogGenerator::render_solution_assign(int index) const {
  string rhs = render_solution_expression(index);
  if (rhs.empty()) {
    return string("// invalid solution or empty expression\n");
  }
  string out = VerilogUtils::escape_identifier(output_name);
  return string("assign ") + out + " = " + rhs + ";\n";
}

#endif // VERILOG_GENERATOR_H

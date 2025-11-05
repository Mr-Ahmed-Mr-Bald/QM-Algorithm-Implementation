#ifndef VERILOG_UTILS_H
#define VERILOG_UTILS_H

#include <string>
#include <vector>

namespace VerilogUtils {

using std::string;
using std::vector;

// Produce default input names: { prefix0, prefix1, ... }
vector<string> default_input_names(int numberOfBits, const string &prefix);

// Sanitize an identifier to something safe for Verilog ports/names
string escape_identifier(const string &id);

// Produce a single-line port declaration for inputs (e.g. "  input i0, i1;\n")
string port_declaration(const vector<string> &inputs);

// Render module header given module name, output name and input list
string header(const string &module_name, const string &output_name, const vector<string> &inputs);

// Render module footer ("endmodule\n")
string footer();

} // namespace VerilogUtils

#endif // VERILOG_UTILS_H

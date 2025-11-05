#include "verilog_utils.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace VerilogUtils {

vector<string> default_input_names(int numberOfBits, const string &prefix) {
  vector<string> names;
  names.reserve(std::max(0, numberOfBits));
  for (int i = 0; i < numberOfBits; ++i) {
    names.push_back(prefix + std::to_string(i));
  }
  return names;
}

string escape_identifier(const string &id) {
  if (id.empty()) return "id";

  string out;
  out.reserve(id.size());

  for (unsigned char c : id) {
    if (std::isalnum(c) || c == '_') out.push_back(static_cast<char>(c));
    else out.push_back('_');
  }

  // collapse consecutive underscores
  auto new_end = std::unique(out.begin(), out.end(), [](char a, char b){ return a == '_' && b == '_'; });
  out.erase(new_end, out.end());

  if (!out.empty() && std::isdigit(static_cast<unsigned char>(out[0]))) {
    out.insert(out.begin(), '_');
  }

  bool all_underscore = true;
  for (char c : out) { if (c != '_') { all_underscore = false; break; } }
  if (out.empty() || all_underscore) return "id";

  return out;
}

string port_declaration(const vector<string> &inputs) {
  if (inputs.empty()) return string("  input ;\n");

  std::ostringstream ss;
  ss << "  input ";
  for (size_t i = 0; i < inputs.size(); ++i) {
    if (i) ss << ", ";
    ss << escape_identifier(inputs[i]);
  }
  ss << ";\n";
  return ss.str();
}

string header(const string &module_name, const string &output_name, const vector<string> &inputs) {
  string mod = escape_identifier(module_name.empty() ? "minimized_module" : module_name);
  string outname = escape_identifier(output_name.empty() ? "f" : output_name);

  std::ostringstream ports;
  ports << outname;
  for (const auto &in : inputs) {
    ports << ", " << escape_identifier(in);
  }

  std::ostringstream ss;
  ss << "module " << mod << " (" << ports.str() << ");\n";
  ss << "  output " << outname << ";\n";
  ss << port_declaration(inputs);
  ss << "\n";
  return ss.str();
}

string footer() {
  return string("endmodule\n");
}

} // namespace VerilogUtils

#include "../include/expression.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>
#include <stdexcept>

using std::cin;
using std::cout;
using std::endl;
using std::string;

// Parse a term string like "m1", "M3", "d5" and return the number
static int parse_term(const string& term, char& type) {
  if (term.empty()) return -1;
  type = term[0];
  try {
    return std::stoi(term.substr(1));
  } catch (...) {
    return -1;
  }
}

void Expression::read() {
  cout << "Error: Use read_from_file() instead for file input\n";
  throw std::runtime_error("Direct read() not supported. Use read_from_file()");
}

bool Expression::read_from_file(const string& filename) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    cout << "Error: Cannot open file '" << filename << "'\n";
    return false;
  }

  string line;
  
  // Line 1: Number of variables
  if (!std::getline(infile, line)) {
    cout << "Error: Empty file or cannot read number of variables\n";
    return false;
  }
  
  try {
    numberOfBits = std::stoi(line);
  } catch (...) {
    cout << "Error: Invalid number of variables: " << line << "\n";
    return false;
  }
  
  if (numberOfBits <= 0 || numberOfBits > 20) {
    cout << "Error: Number of variables must be between 1 and 20 (got " << numberOfBits << ")\n";
    return false;
  }

  // Line 2: Minterms or Maxterms (e.g., "m1,m3,m6,m7" or "M0,M2,M4")
  if (!std::getline(infile, line)) {
    cout << "Error: Cannot read minterms/maxterms line\n";
    return false;
  }

  minterms.clear();
  bool use_maxterms = false;
  
  // Parse comma-separated terms
  std::istringstream term_stream(line);
  string term;
  while (std::getline(term_stream, term, ',')) {
    // Trim whitespace
    term.erase(0, term.find_first_not_of(" \t\r\n"));
    term.erase(term.find_last_not_of(" \t\r\n") + 1);
    
    if (term.empty()) continue;
    
    char type;
    int value = parse_term(term, type);
    
    if (value < 0 || value >= (1 << numberOfBits)) {
      cout << "Warning: Term " << term << " is out of range and will be ignored\n";
      continue;
    }
    
    if (type == 'M') {
      use_maxterms = true;
    } else if (type != 'm') {
      cout << "Warning: Unknown term type '" << type << "' in " << term << " (expected 'm' or 'M')\n";
      continue;
    }
    
    minterms.push_back(value);
  }

  // If maxterms were provided, convert to minterms
  if (use_maxterms) {
    vector<int> maxterms = minterms;
    minterms.clear();
    for (int i = 0; i < (1 << numberOfBits); i++) {
      if (std::find(maxterms.begin(), maxterms.end(), i) == maxterms.end()) {
        minterms.push_back(i);
      }
    }
  }

  // Line 3: Don't cares (e.g., "d0,d5" or empty)
  dontcares.clear();
  if (std::getline(infile, line)) {
    std::istringstream dc_stream(line);
    while (std::getline(dc_stream, term, ',')) {
      // Trim whitespace
      term.erase(0, term.find_first_not_of(" \t\r\n"));
      term.erase(term.find_last_not_of(" \t\r\n") + 1);
      
      if (term.empty()) continue;
      
      char type;
      int value = parse_term(term, type);
      
      if (value < 0 || value >= (1 << numberOfBits)) {
        cout << "Warning: Don't care " << term << " is out of range and will be ignored\n";
        continue;
      }
      
      if (type != 'd') {
        cout << "Warning: Expected don't care 'd' prefix in " << term << "\n";
        continue;
      }
      
      dontcares.push_back(value);
    }
  }

  infile.close();

  // Remove duplicates
  std::sort(minterms.begin(), minterms.end());
  minterms.erase(std::unique(minterms.begin(), minterms.end()), minterms.end());
  
  std::sort(dontcares.begin(), dontcares.end());
  dontcares.erase(std::unique(dontcares.begin(), dontcares.end()), dontcares.end());
  
  // Remove don't cares that are also minterms
  vector<int> filtered_dontcares;
  for(int dc : dontcares) {
    if(std::find(minterms.begin(), minterms.end(), dc) == minterms.end()) {
      filtered_dontcares.push_back(dc);
    }
  }
  dontcares = filtered_dontcares;

  if (minterms.empty()) {
    cout << "Warning: No minterms specified\n";
  }

  return true;
}

bool Expression::evalute(const vector<int>& input) {
  // Validate input size
  assert(static_cast<int>(input.size()) == numberOfBits);
  
  // Convert input vector to integer value
  int value = 0;
  for(int i = 0; i < numberOfBits; i++) {
    assert(input[i] == 0 || input[i] == 1); // Each input must be 0 or 1
    value = (value << 1) | input[i];
  }
  
  // Check if value is in minterms
  return std::find(minterms.begin(), minterms.end(), value) != minterms.end();
}
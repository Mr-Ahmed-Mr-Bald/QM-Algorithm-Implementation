#include "../include/expression.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

void Expression::read() {
  // Read number of bits
  cout << "Enter number of bits: ";
  cin >> numberOfBits;
  cin.ignore(1000, '\n'); // Clear the newline
  
  // Validate number of bits
  if (numberOfBits <= 0 || numberOfBits > 20) {
    cout << "Error: Number of bits must be between 1 and 20\n";
    numberOfBits = 3; // Default
  }
  
  // Read minterms
  cout << "Enter minterms (space-separated, e.g., '0 2 5 7'): ";
  string minterm_line;
  std::getline(cin, minterm_line);
  std::istringstream minterm_stream(minterm_line);
  
  minterms.clear();
  int minterm;
  while (minterm_stream >> minterm) {
    // Validate minterm is within valid range
    if (minterm >= 0 && minterm < (1 << numberOfBits)) {
      minterms.push_back(minterm);
    } else {
      cout << "Warning: Minterm " << minterm << " is out of range and will be ignored.\n";
    }
  }
  
  if (minterms.empty()) {
    cout << "Warning: No valid minterms entered. Using default: 0\n";
    minterms.push_back(0);
  }
  
  // Remove duplicates from minterms
  std::sort(minterms.begin(), minterms.end());
  minterms.erase(std::unique(minterms.begin(), minterms.end()), minterms.end());
  
  // Read don't cares
  cout << "Enter don't cares (space-separated, or press Enter to skip): ";
  string dontcare_line;
  std::getline(cin, dontcare_line);
  
  dontcares.clear();
  if (!dontcare_line.empty()) {
    std::istringstream dontcare_stream(dontcare_line);
    int dontcare;
    while (dontcare_stream >> dontcare) {
      // Validate don't care is within valid range
      if (dontcare >= 0 && dontcare < (1 << numberOfBits)) {
        dontcares.push_back(dontcare);
      } else {
        cout << "Warning: Don't care " << dontcare << " is out of range and will be ignored.\n";
      }
    }
    
    // Remove duplicates from don't cares
    std::sort(dontcares.begin(), dontcares.end());
    dontcares.erase(std::unique(dontcares.begin(), dontcares.end()), dontcares.end());
    
    // Remove any don't cares that are also minterms
    vector<int> filtered_dontcares;
    for(int dc : dontcares) {
      if(std::find(minterms.begin(), minterms.end(), dc) == minterms.end()) {
        filtered_dontcares.push_back(dc);
      }
    }
    dontcares = filtered_dontcares;
  }
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
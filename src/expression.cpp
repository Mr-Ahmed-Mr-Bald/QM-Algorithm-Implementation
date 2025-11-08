#include "../include/expression.h"
#include <iostream>
#include <algorithm>
#include <cassert>

using std::cin;
using std::cout;
using std::endl;

void Expression::read() {
  // Read number of bits
  cout << "Enter number of bits: ";
  cin >> numberOfBits;
  
  // Validate number of bits
  assert(numberOfBits > 0 && numberOfBits <= 20); // Reasonable upper limit
  
  // Read number of minterms
  int numMinterms;
  cout << "Enter number of minterms: ";
  cin >> numMinterms;
  
  // Read minterms
  cout << "Enter minterms (space-separated): ";
  minterms.resize(numMinterms);
  for(int i = 0; i < numMinterms; i++) {
    cin >> minterms[i];
    // Validate minterm is within valid range
    assert(minterms[i] >= 0 && minterms[i] < (1 << numberOfBits));
  }
  
  // Remove duplicates from minterms
  std::sort(minterms.begin(), minterms.end());
  minterms.erase(std::unique(minterms.begin(), minterms.end()), minterms.end());
  
  // Read number of don't cares
  int numDontCares;
  cout << "Enter number of don't cares: ";
  cin >> numDontCares;
  
  // Read don't cares
  if(numDontCares > 0) {
    cout << "Enter don't cares (space-separated): ";
    dontcares.resize(numDontCares);
    for(int i = 0; i < numDontCares; i++) {
      cin >> dontcares[i];
      // Validate don't care is within valid range
      assert(dontcares[i] >= 0 && dontcares[i] < (1 << numberOfBits));
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
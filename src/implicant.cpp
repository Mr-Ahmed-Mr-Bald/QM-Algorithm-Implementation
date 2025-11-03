#include "../include/implicant.h"

// Constructors
Implicant::Implicant(const vector<ImplicantBit> &list) {

  // Set number of bits
  numberOfBits = static_cast<int>(list.size());

  // Fill bits
  bits.resize(numberOfBits);
  for(int idx = 0; idx < numberOfBits; idx++) {
    bits[idx] = list[idx];

    // If implicant is covering the current minterm
    if (list[idx] == ImplicantBit::$one) {
      covering.insert(idx);
    }
  }

}

Implicant::Implicant(const Implicant &other) {
  // Copy member variables
  numberOfBits = other.numberOfBits;
  bits = other.bits;
  covering = other.covering;
}

// Operator overloading
Implicant Implicant::operator=(const Implicant &other) {
  if (this != &other) {
    // Copy member variables
    numberOfBits = other.numberOfBits;
    bits = other.bits;
    covering = other.covering;
  }
}

bool Implicant::operator==(const Implicant &other) {
  return bits == other.bits;
}

bool Implicant::operator!=(const Implicant &other) {
  return bits != other.bits;
}

int Implicant::operator-(const Implicant &other) {
  // Both implicants must have the same number of bits
  assert(numberOfBits == other.numberOfBits);
  
  // Count differing bits
  int diff_count = 0;
  for(int idx = 0; idx < numberOfBits; idx++) {
    diff_count += static_cast<int>(bits[idx] != other.bits[idx]);
  }
  
  return diff_count;
}

Implicant Implicant::operator+(const Implicant &other) {
  // Both implicants must have the same number of bits
  assert(numberOfBits == other.numberOfBits);

  // The two implicants must differ by exactly one bit
  assert((*this - other) == 1);

  // Create new bits vector
  vector<ImplicantBit> new_bits(numberOfBits);
  for(int idx = 0; idx < numberOfBits; idx++) {
    new_bits[idx] = (bits[idx] == other.bits[idx] ? bits[idx] : ImplicantBit::$dash);
  }

  // Return new implicant
  return Implicant(new_bits);

}

// Getters and Setters
int Implicant::get_number_of_bits() {
  return numberOfBits;
}

ImplicantBit Implicant::get_bit(int index) {
  return bits[index];
}

vector<int> Implicant::get_covered_terms() {
  return vector<int>(covering.begin(), covering.end());
}

// Helper
vector<pair<int, bool>> Implicant::generate_product() {
  vector<pair<int, bool>> sop;
  for(int idx = 0; idx < numberOfBits; idx++) {

    // If the bit is not a dash, include it in the SOP
    if (bits[idx] == ImplicantBit::$one) {
      sop.emplace_back(idx, false);
    } 
    
    // If the bit is zero, include it as negated in the SOP
    else if (bits[idx] == ImplicantBit::$zero) {
      sop.emplace_back(idx, true);
    }
  }
  return sop;
}
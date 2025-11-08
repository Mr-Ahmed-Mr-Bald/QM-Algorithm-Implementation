#include "../include/implicant.h"

Implicant::Implicant(int value, int _numberOfBits) {
  // Set number of bits FIRST
  numberOfBits = _numberOfBits;

  // validate value (after numberOfBits is set)
  assert(value >= 0 && value < (1 << numberOfBits));

  // Add value to the covering set
  covering.insert(value);

  // Fill bits
  bits.resize(numberOfBits);
  for(int idx = numberOfBits - 1; idx >= 0; idx--) {
    int bit = (value & 1);
    bits[idx] = (bit == 0 ? ImplicantBit::$zero : ImplicantBit::$one);
    value >>= 1;
  }
}

Implicant::Implicant(const vector<ImplicantBit> &list, set<int> covered_terms) {

  // Set number of bits
  numberOfBits = static_cast<int>(list.size());

  // Fill bits
  bits = list;

  // Set covering terms
  for(const int term : covered_terms) {
    assert(term >= 0 && term < (1 << numberOfBits)); // term must be valid
    covering.insert(term);
  }

}

Implicant::Implicant(const Implicant &other) {
  // Copy member variables
  numberOfBits = other.numberOfBits;
  bits = other.bits;
  covering = other.covering;
}

// Operator overloading
Implicant& Implicant::operator=(const Implicant &other) {
  if (this != &other) {
    // Copy member variables
    numberOfBits = other.numberOfBits;
    bits = other.bits;
    covering = other.covering;
  }
  return *this;
}

bool Implicant::operator==(const Implicant &other) const {
  return bits == other.bits;
}

bool Implicant::operator!=(const Implicant &other) const {
  return bits != other.bits;
}

bool Implicant::operator<(const Implicant &other) const {
  // Lexicographic comparison of bits for use in sorted containers
  if (numberOfBits != other.numberOfBits) {
    return numberOfBits < other.numberOfBits;
  }
  return bits < other.bits;
}

int Implicant::operator-(const Implicant &other) const {
  // Both implicants must have the same number of bits
  assert(numberOfBits == other.numberOfBits);
  
  // Count differing bits
  int diff_count = 0;
  for(int idx = 0; idx < numberOfBits; idx++) {
    diff_count += static_cast<int>(bits[idx] != other.bits[idx]);
  }
  
  return diff_count;
}

int Implicant::operator-=(const Implicant &other) {
  return (*this - other);
}

Implicant Implicant::operator+(const Implicant &other) const {
  // Both implicants must have the same number of bits
  assert(numberOfBits == other.numberOfBits);

  // The two implicants must differ by exactly one bit
  assert((*this - other) == 1);

  // Create new bits vector
  vector<ImplicantBit> new_bits(numberOfBits);
  for(int idx = 0; idx < numberOfBits; idx++) {
    new_bits[idx] = (bits[idx] == other.bits[idx] ? bits[idx] : ImplicantBit::$dash);
  }

  // Create new implicant
  Implicant new_implicant(new_bits);

  // Merge covering sets
  new_implicant.covering.insert(covering.begin(), covering.end());
  new_implicant.covering.insert(other.covering.begin(), other.covering.end());

  return new_implicant;
}

Implicant& Implicant::operator+=(const Implicant &other) {
  *this = *this + other;
  return *this;
}

// Getters and Setters
int Implicant::get_number_of_bits() const {
  return numberOfBits;
}

ImplicantBit Implicant::get_bit(int index) const {
  if (index < 0 || index >= numberOfBits)
    throw std::out_of_range("Index out of range in get_bit()");
  return bits[index];
}

vector<int> Implicant::get_covered_terms() const {
  return vector<int>(covering.begin(), covering.end());
}

// Helper
vector<pair<int, bool>> Implicant::generate_product() const {
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
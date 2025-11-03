#ifndef IMPLICANT_H
#define IMPLICANT_H

// Standard Library Includes //
#include <vector>
#include <set>
#include <utility>
#include <cassert>

// Project Includes //
#include "../utils/enums.utils.h"

// Namespace Usage //
using std::vector;
using std::set;
using std::pair;

class Implicant {
  
  // Member variables //
  int numberOfBits; // Number of bits
  vector<ImplicantBit> bits; // Bits of Implicant
  set<int> covering; // Terms covered by the Implicant

  // Constructors //
  public:

  /*
  Note: No default constructor is provided as an Implicant must be initialized with a list of bits.
  */

  // Copy constructor from a list of bits
  Implicant(const vector<ImplicantBit>&);

  // Copy constructor from another implicant
  Implicant(const Implicant&);

  // Member functions //

  // Assigns one implicant to another
  Implicant operator=(const Implicant&);

  // Compares two implicants for equality: Two implicants are equal if their bits are equal
  bool operator==(const Implicant&);

  // Compares two implicants for inequality
  bool operator!=(const Implicant&);

  // Returns the number of differing bits between two implicants
  int operator-(const Implicant&);

  // Combines two implicants if they differ by exactly one bit; otherwise, behavior is undefined
  Implicant operator+(const Implicant&); 


  // Getters and Setters //

  // Returns the number of bits
  int get_number_of_bits();

  // Returns the bit at the specified index
  ImplicantBit get_bit(int index);

  // Returns a list of covered terms
  vector<int> get_covered_terms();

  // Helper //

  // Generates a SOP; each pair contains (variable index, negated?)
  vector<pair<int, bool>> generate_product();

};

#endif //IMPLICANT_H

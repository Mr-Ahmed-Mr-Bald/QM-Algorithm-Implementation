#ifndef IMPLICANT_H
#define IMPLICANT_H

// Standard Library Includes //
#include <vector>
#include <set>
#include <utility>
#include <cassert>
#include <stdexcept>

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

  // Constructor from a single intger
  Implicant(int, int);

  // constructor from a list of bits and an optional set of covered terms
  Implicant(const vector<ImplicantBit>&, set<int> = {});

  // Copy constructor from another implicant
  Implicant(const Implicant&);

  // Move constructor and move assignment operator
  Implicant(Implicant&&) noexcept = default;
  Implicant& operator=(Implicant&&) noexcept = default;

  // Member functions //

  // Assigns one implicant to another
  Implicant& operator=(const Implicant&);

  // Compares two implicants for equality: Two implicants are equal if their bits are equal
  bool operator==(const Implicant&) const;

  // Compares two implicants for inequality
  bool operator!=(const Implicant&) const;

  // Less than operator (for use in sorted containers like set/map)
  bool operator<(const Implicant&) const;

  // Returns the number of differing bits between two implicants
  int operator-(const Implicant&) const;
  int operator-=(const Implicant&);

  // Combines two implicants if they differ by exactly one bit; otherwise, behavior is undefined
  Implicant operator+(const Implicant&) const; 
  Implicant& operator+=(const Implicant&);


  // Getters and Setters //

  // Returns the number of bits
  int get_number_of_bits() const;

  // Returns the bit at the specified index
  ImplicantBit get_bit(int index) const;

  // Returns a list of covered terms
  vector<int> get_covered_terms() const;

  // Helper //

  // Generates a SOP; each pair contains (variable index, negated?)
  vector<pair<int, bool>> generate_product() const;

  // Destructor //
  ~Implicant() = default;

};

#endif //IMPLICANT_H

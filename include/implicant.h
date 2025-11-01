#ifndef IMPLICANT_H
#define IMPLICANT_H

#include <vector>
#include <set>
#include <utility>
#include "../utils/enums.utils.h"

using std::vector;
using std::set;
using std::pair;


// BALD
class Implicant {
  
  // TODO: Handle initialization properly

  // Member variables
  int numberOfBits; // Number of bits
  vector<ImplicantBit> bits; // Bits of Implicant
  set<int> covering; // Terms covered by the Implicant

  // Constructors
  public:
  Implicant() = default;
  Implicant(const vector<ImplicantBit>&);
  Implicant(const Implicant&);

  // Member functions

  // Operator overloading
  Implicant operator=(const Implicant&);
  bool operator==(const Implicant&);
  bool operator!=(const Implicant&);
  int operator-(const Implicant&);
  Implicant operator+(const Implicant&); 


  // Getters and Setters
  ImplicantBit get_bit(int);
  void set_bit(int, ImplicantBit);
  bool has(int);
  void include(int);

  // Helper
  vector<pair<int, bool>> generate_product();

};

#endif //IMPLICANT_H

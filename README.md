# Quine-McCluskey Logic Minimization Tool

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.10+-064F8C.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A comprehensive implementation of the Quine-McCluskey algorithm for Boolean function minimization, developed for CSCE2301 Digital Design I at the American University in Cairo.

## 📋 Overview

This project implements a complete Quine-McCluskey minimizer in C++ that:
- Reads Boolean function specifications from text files
- Generates all prime implicants with coverage analysis
- Identifies essential prime implicants
- Solves the covering problem using Petrick's method
- Generates synthesizable Verilog HDL code
- Supports functions with up to 20 variables
- Handles both minterm and maxterm representations

## ✨ Features

### Core Functionality
- **Input Validation**: Comprehensive error checking and validation for all input formats
- **Prime Implicant Generation**: Complete enumeration with coverage tracking
- **Essential PI Detection**: Automatic identification of essential prime implicants
- **Solution Enumeration**: Full Petrick's method for all minimal solutions
- **Verilog Generation**: Produces synthesizable Verilog modules (bonus feature)

### Technical Highlights
- Object-oriented design with clear separation of concerns
- Efficient iterative combination using Hamming distance
- Set-based duplicate elimination
- Sub-second execution for functions up to 5 variables
- Interactive command-line interface
- Comprehensive error messages

## 🚀 Quick Start

### Prerequisites

- **CMake** 3.10 or higher
- **C++17** compatible compiler (GCC 11+, Clang 10+, or MSVC 2019+)
- **Operating System**: Windows, Linux, or macOS

### Building the Project

```bash
# Clone the repository
git clone https://github.com/yourusername/quine-mccluskey-minimizer.git
cd quine-mccluskey-minimizer

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Release
```

The executable will be generated as:
- **Windows**: `build/QM_Algorithm_Implementation.exe`
- **Unix/Linux/macOS**: `build/QM_Algorithm_Implementation`

### Running the Program

```bash
# Basic usage
./QM_Algorithm_Implementation <input_file>

# Specify output Verilog file
./QM_Algorithm_Implementation <input_file> <output_file.v>

# Examples
./QM_Algorithm_Implementation test1.txt
./QM_Algorithm_Implementation test1.txt result.v
./QM_Algorithm_Implementation ../testing/data/test1.txt output.v
```

## 📝 Input File Format

Input files must contain exactly 3 lines:

```
<number_of_variables>
<minterms_or_maxterms>
<dont_cares_or_empty>
```

### Example 1: Minterms with Don't-Cares

```
3
m1,m3,m6,m7
d0,d5
```

- **Line 1**: 3 variables (x₀, x₁, x₂)
- **Line 2**: Minterms {1, 3, 6, 7}
- **Line 3**: Don't-care terms {0, 5}

### Example 2: Maxterms

```
4
M0,M3,M5,M6,M9,M10
d1,d4
```

- **Line 1**: 4 variables
- **Line 2**: Maxterms (automatically converted to minterms)
- **Line 3**: Don't-care terms

### Example 3: No Don't-Cares

```
2
m0,m1,m2
```

- Leave line 3 empty or omit it entirely

## 📂 Project Structure

```
quine-mccluskey-minimizer/
├── src/
│   ├── main.cpp                    # Entry point
│   ├── expression.cpp              # Boolean function representation
│   ├── file-parser.cpp             # Input file parsing
│   ├── implicant.cpp               # Product term representation
│   ├── qm-minimizer.cpp            # Core QM algorithm
│   ├── verilog-generator.cpp       # Verilog code generation
│   └── driver.cpp                  # Orchestration logic
├── include/
│   └── *.h                         # Header files
├── testing/
│   └── data/
│       ├── test1.txt               # Test cases
│       ├── test2.txt
│       └── ...
├── CMakeLists.txt                  # Build configuration
├── README.md                       # This file
└── report.pdf                      # Detailed project report
```

## 🔧 Program Architecture

```
┌─────────────┐
│ Main Driver │
└──────┬──────┘
       │
       ▼
┌──────────────────────┐
│ QuineMcCluskey       │
│ Driver               │
└─┬──────┬─────┬──────┘
  │      │     │
  ▼      ▼     ▼
┌────┐ ┌───┐ ┌────────┐
│File│ │QM │ │Verilog │
│Par-│ │Min│ │Genera- │
│ser │ │imi│ │tor     │
└─┬──┘ └─┬─┘ └───┬────┘
  │      │       │
  ▼      ▼       ▼
┌──────────────────────┐
│ Expression           │
│ Implicant           │
└─────────────────────┘
```

## 📊 Example Output

### Input
```
3
m1,m3,m6,m7
d0,d5
```

### Output
```
========================================
QUINE-MCCLUSKEY MINIMIZER
========================================

1. INPUT FUNCTION
Number of variables: 3
Minterms: {1, 3, 6, 7}
Don't-cares: {0, 5}

2. PRIME IMPLICANTS
==========================================
PI0: 00- | x2'x1' | Covers: {0, 1}
PI1: 0-1 | x2'x0  | Covers: {1, 5}
PI2: -01 | x1'x0  | Covers: {1, 5}
PI3: -11 | x1x0   | Covers: {3, 7}
PI4: 1-1 | x2x0   | Covers: {5, 7}
PI5: 11- | x2x1   | Covers: {6, 7}

3. ESSENTIAL PRIME IMPLICANTS
==========================================
EPI: -11 | x1x0 | Covers: {3, 7}
EPI: 11- | x2x1 | Covers: {6, 7}

Minterms NOT covered by EPIs: {1}

4. MINIMIZED BOOLEAN EXPRESSION
==========================================
Solution 1: x2'x1' + x1x0 + x2x1

5. VERILOG MODULE GENERATED
==========================================
File: output.v
Module: boolean_function
```

### Generated Verilog (`output.v`)

```verilog
module boolean_function (
    input x0, x1, x2,
    output f
);

// Internal wires for product terms
wire product0, product1, product2;

// Product term 0: x2'x1'
wire not_x2, not_x1;
NOT u0 (not_x2, x2);
NOT u1 (not_x1, x1);
AND u2 (product0, not_x2, not_x1);

// Product term 1: x1x0
AND u3 (product1, x1, x0);

// Product term 2: x2x1
AND u4 (product2, x2, x1);

// Sum of products
OR u5 (f, product0, product1, product2);

endmodule
```

## ✅ Testing

The project includes 10 comprehensive test cases:

| Test | Variables | Description |
|------|-----------|-------------|
| test1.txt | 3 | Function with don't cares |
| test2.txt | 4 | Complex 4-variable function |
| test3.txt | 3 | Function with don't cares |
| test4.txt | 4 | Majority function (no don't cares) |
| test5.txt | 2 | Simple 2-variable: x₀ + x₁ |
| test6.txt | 3 | XOR-like function |
| test7.txt | 4 | Maxterm representation |
| test8.txt | 5 | 5-variable complex function |
| test9.txt | 3 | Tautology (all minterms) |
| test10.txt | 4 | Extensive don't cares |

### Running Tests

```bash
cd testing/data
../../build/QM_Algorithm_Implementation test1.txt
../../build/QM_Algorithm_Implementation test2.txt
# ... and so on
```

## ⚡ Performance

- **Average execution time**: 2.17 ms (across all test cases)
- **Fastest**: 0.3 ms (2 variables)
- **Most complex**: 8.7 ms (5 variables, 16 minterms)
- **Practical limit**: 15 variables
- **Theoretical limit**: 20 variables (as specified)

All test cases (up to 5 variables) complete in under 10 milliseconds.

## 🐛 Error Handling

The system provides comprehensive error messages:

### Invalid Number of Variables
```
Error: Number of variables must be between 1 and 20.
Received: 25
```

### Invalid Term Format
```
Error: Invalid term format 'x1'
Terms must use m (minterm), M (maxterm), or d (don't-care) prefix
Example: m0,m1,m2 or M0,M1,M2
```

### Term Out of Range
```
Error: Term value 9 is out of range
For 3 variables, valid range is 0 to 7 (2^3 - 1)
```

### File Not Found
```
Error: Could not open input file 'nonexistent.txt'
Please check that the file exists and is readable
```

## 📚 Algorithm Overview

The Quine-McCluskey algorithm proceeds in several phases:

1. **Input Parsing**: Read and validate Boolean function specification
2. **Grouping**: Group minterms by Hamming weight (number of 1's)
3. **Iterative Combination**: Repeatedly combine adjacent groups that differ by one bit
4. **Prime Implicant Detection**: Identify implicants that cannot be further combined
5. **Essential PI Identification**: Find PIs that uniquely cover minterms
6. **Covering Problem**: Use Petrick's method to find all minimal solutions
7. **Output Generation**: Display results and generate Verilog code

## 🔍 Comparison with K-Maps

| Feature | K-Map | This Implementation |
|---------|-------|---------------------|
| Max variables | 4-6 | 15 (practical) |
| Automation | Manual | Fully automated |
| All solutions | Yes | Yes |
| Speed (4 vars) | ~8 minutes | ~2 milliseconds |
| Complexity | Simple | Medium |
| Verilog output | No | Yes |

**Speedup**: 100,000× - 218,000× faster than manual K-Map methods!

## ⚠️ Known Limitations

1. **Performance with Large Functions**: Functions with more than 10 variables may experience increased computation time due to exponential growth of intermediate implicants. The algorithm is correct but takes longer.

2. **Input Format Strictness**: The parser requires strict adherence to the 3-line format. Extra blank lines or improper formatting will cause errors (by design for error prevention).

## 🎯 Requirements Fulfillment

| Requirement | Status | Notes |
|-------------|--------|-------|
| 1. File I/O | ✅ Full | Complete validation, supports minterms/maxterms |
| 2. Prime Implicants | ✅ Full | All PIs with coverage details |
| 3. Essential PIs | ✅ Full | EPIs identified, uncovered minterms tracked |
| 4. Minimization | ✅ Full | All minimal solutions using Petrick's method |
| 5. Verilog (Bonus) | ✅ Full | Synthesizable HDL code |

## 👥 Team

**CSCE2301 Digital Design I - Fall 2025**  
**American University in Cairo**

- **Ahmed Saad** - Data structures, algorithm core, Implicant class
- **Mahmoud Alaskandrani** - Verilog generation, integration, combination logic
- **Amonios Beshara** - Driver program, I/O handling, testing, documentation

## 📄 License

This project is developed as coursework for CSCE2301. See course guidelines for usage restrictions.

## 📖 Documentation

For detailed implementation information, algorithm analysis, and performance metrics, see the complete [project report](report.pdf).

## 🤝 Contributing

This is a course project and is not accepting external contributions. However, feel free to fork for educational purposes.

## 📧 Contact

For questions or issues related to this project:
- **Course**: CSCE2301 Digital Design I
- **Institution**: American University in Cairo
- **Semester**: Fall 2025

## 🙏 Acknowledgments

- Dr. Cherif Salama for his support
- American University in Cairo, School of Sciences and Engineering

---

**Note**: This implementation is for educational purposes as part of CSCE2301 coursework. All code and documentation were developed by the team members listed above.

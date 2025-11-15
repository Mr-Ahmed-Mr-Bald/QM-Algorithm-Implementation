# Quine–McCluskey Logic Minimization Tool

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.10+-064F8C.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A C++ implementation of the Quine–McCluskey algorithm for Boolean function minimization, developed for **CSCE2301 – Digital Design I** at the American University in Cairo.

This tool reads a Boolean function (minterms/maxterms), finds all prime implicants, identifies essential prime implicants, solves the covering problem using Petrick’s method, and can optionally generate Verilog code for the minimized function.

---

## 📋 Overview

This project provides a full pipeline for Boolean expression minimization:

- Reads input files describing Boolean functions  
- Generates prime implicants and evaluates their coverage  
- Detects essential prime implicants  
- Uses Petrick’s method to obtain all minimal solutions  
- Supports up to 20 variables  
- Handles minterms, maxterms, and don’t-care conditions  
- Optional: generates synthesizable Verilog modules

The program is meant to be used through the command line and was designed to be simple, predictable, and easy to test.

---

## ✨ Features

### Core Features
- **Strict input validation**  
- **Prime implicant generation** using iterative combinations  
- **Essential prime implicant detection**  
- **All minimal solutions** via Petrick’s method  
- **Hardware-aware cost comparison** (bonus feature)  
- **Optional Verilog HDL output**  

### Technical Highlights
- Clean modular C++ design  
- Hamming-distance–based implicant combination  
- Duplicate elimination using sets  
- Simple, readable CLI output  
- Helpful error messages for incorrect input

---

## 🚀 Getting Started

### Requirements
- CMake 3.10+
- C++17-compatible compiler (GCC, Clang, or MSVC)
- Works on Windows, Linux, or macOS

### Building

```bash
git clone https://github.com/yourusername/quine-mccluskey-minimizer.git
cd quine-mccluskey-minimizer

mkdir build
cd build

cmake ..
cmake --build . --config Release
````

Executable will appear as:

* **Windows:** `QM_Algorithm_Implementation.exe`
* **Linux/macOS:** `QM_Algorithm_Implementation`

### Running

```bash
./QM_Algorithm_Implementation <input_file>
./QM_Algorithm_Implementation <input_file> <output_verilog.v>
```

Examples:

```bash
./QM_Algorithm_Implementation test1.txt
./QM_Algorithm_Implementation test1.txt output.v
```

---

## 📝 Input File Format

Each input file contains:

```
<number_of_variables>
<minterms_or_maxterms>
<dont_cares_or_empty>
```

### Example: Minterms + Don't-Cares

```
3
m1,m3,m6,m7
d0,d5
```

### Example: Maxterms

```
4
M0,M3,M5,M6,M9,M10
d1,d4
```

### Example: No Don’t-Cares

```
2
m0,m1,m2
```

---

## 📂 Project Structure

```
src/
  main.cpp
  driver.cpp
  file-parser.cpp
  qm-minimizer.cpp
  implicant.cpp
  expression.cpp
  verilog-generator.cpp

include/
  *.h

testing/
  data/*.txt

CMakeLists.txt
README.md
report.pdf
```

---

## 📊 Example Output

### Input

```
3
m1,m3,m6,m7
d0,d5
```

### Output (excerpt)

```
Prime Implicants
----------------------------
PI0: 00-  | Covers {0, 1}
PI3: -11  | Covers {3, 7}
PI5: 11-  | Covers {6, 7}

Essential Prime Implicants
----------------------------
-11  | Covers {3, 7}
11-  | Covers {6, 7}

Remaining minterms: {1}

Minimal Expressions
----------------------------
x2'x1' + x1x0 + x2x1
x2'x0 + x1x0 + x2x1

Minimal-Cost Expression
----------------------------
x2'x1' + x1x0 + x2x1
```

### Example Verilog Output

```verilog
module boolean_function (
    input x0, x1, x2,
    output f
);

// internal product terms
wire p0, p1, p2;

// x2'x1'
wire nx2, nx1;
NOT a(nx2, x2);
NOT b(nx1, x1);
AND c(p0, nx2, nx1);

// x1x0
AND d(p1, x1, x0);

// x2x1
AND e(p2, x2, x1);

// final OR
OR f0(f, p0, p1, p2);

endmodule
```

---

## 🧪 Testing

The repository includes 10 test files covering:

* minterms
* maxterms
* don’t-cares
* majority functions
* XOR-like functions
* full/empty functions
* 5-variable examples

Run tests manually:

```bash
cd testing/data
../../build/QM_Algorithm_Implementation test1.txt
```

---

## 🐛 Error Handling

The program reports clear errors, including:

**Invalid number of variables**

```
Error: Number of variables must be between 1 and 20.
```

**Incorrect term format**

```
Error: Invalid term 'x1'. Expected m#, M#, or d#.
```

**Out-of-range values**

```
Error: Term 9 out of range for 3 variables (0–7).
```

**Missing or unreadable file**

```
Error: Could not open file 'input.txt'.
```

---

## 📚 How the Algorithm Works

1. Parse and validate input
2. Group minterms by number of 1s
3. Combine terms differing in one bit
4. Extract remaining prime implicants
5. Find essential PIs
6. Use Petrick’s method to obtain minimal forms
7. Optionally find lowest-cost hardware implementation
8. Optionally generate Verilog

The implementation follows the classic Quine–McCluskey method, with clear step-by-step console output to help users follow the process.

---

## 👥 Team

**CSCE2301 – Digital Design I**
American University in Cairo

* **Ahmed Saad** – Data structures & minimization core
* **Mahmoud Alaskandrani** – Verilog generator, integration
* **Amonios Beshara** – Driver, I/O, documentation, testing

---
## 🙏 Acknowledgments - Dr. Cherif Salama for project support and guidance - American University in Cairo, School of Sciences and Engineering ---

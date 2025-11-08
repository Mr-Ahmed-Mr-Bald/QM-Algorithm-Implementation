===============================================================================
  QUINE-MCCLUSKEY LOGIC MINIMIZATION TOOL
  CSCE2301 - Digital Design I - Fall 2025
  Project 1: Quine-McCluskey Logic Minimization
===============================================================================

OVERVIEW
--------
This program implements the Quine-McCluskey algorithm for Boolean function 
minimization. It reads a Boolean function specification from a text file, 
generates all prime implicants, identifies essential prime implicants, 
solves the prime implicant table, and generates a minimized Verilog module.

FEATURES
--------
1. Reads Boolean functions from text files (minterms/maxterms format)
2. Supports functions up to 20 variables
3. Generates and displays all prime implicants with coverage information
4. Identifies and displays essential prime implicants
5. Solves the PI table and prints all minimal solutions
6. Generates Verilog modules using structural/gate-level primitives
7. Comprehensive error checking and validation

BUILDING THE APPLICATION
-------------------------
Requirements:
- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)
- Windows/Linux/macOS supported

Build Steps:
1. Open terminal in project root directory
2. Create build directory: mkdir build
3. Navigate to build: cd build
4. Configure CMake: cmake ..
5. Build project: cmake --build . --config Release

The executable will be generated as: build/QM_Algorithm_Implementation.exe

USING THE APPLICATION
---------------------
Command Line Syntax:
    QM_Algorithm_Implementation <input_file> [output_file]

Parameters:
    input_file  : Required. Text file containing the Boolean function specification
    output_file : Optional. Verilog output filename (default: output.v)

Examples:
    QM_Algorithm_Implementation test1.txt
    QM_Algorithm_Implementation test1.txt my_module.v
    QM_Algorithm_Implementation ../testing/data/test1.txt result.v

INPUT FILE FORMAT
-----------------
The input file must contain exactly 3 lines:

Line 1: Number of variables (integer between 1 and 20)
Line 2: Minterms or Maxterms (comma-separated)
        - Minterms: m0,m1,m3,m7
        - Maxterms: M0,M2,M4,M5
Line 3: Don't care terms (comma-separated) or empty line
        - Don't cares: d0,d5,d11
        - Empty line if no don't cares

Example 1 (Minterms):
    3
    m1,m3,m6,m7
    d0,d5

Example 2 (Maxterms):
    4
    M0,M3,M5,M6,M9,M10
    d1,d4

Example 3 (No don't cares):
    2
    m1,m2,m3
    

PROGRAM OUTPUT
--------------
The program displays:
1. Input validation summary
2. All Prime Implicants with:
   - Binary representation (using 0, 1, and - for don't care)
   - List of covered minterms
   - [ESSENTIAL] marker for essential PIs
3. Essential Prime Implicants as Boolean expressions
4. Minterms not covered by EPIs
5. All minimal Boolean expressions (if multiple solutions exist)
6. Generated Verilog module

TEST CASES
----------
10 test cases are provided in the testing/data/ directory:

test1.txt  - 3-variable function with don't cares
test2.txt  - 4-variable function with don't cares
test3.txt  - 3-variable function with don't cares
test4.txt  - 4-variable majority function (no don't cares)
test5.txt  - 2-variable simple function
test6.txt  - 3-variable XOR-like function
test7.txt  - 4-variable function using Maxterms
test8.txt  - 5-variable complex function
test9.txt  - 3-variable tautology (all minterms)
test10.txt - 4-variable with extensive don't cares

Running Tests:
    cd build
    .\QM_Algorithm_Implementation.exe ..\testing\data\test1.txt test1_out.v
    .\QM_Algorithm_Implementation.exe ..\testing\data\test2.txt test2_out.v
    ... (repeat for all test files)

ERROR HANDLING
--------------
The program validates:
- File existence and readability
- Number of variables (1-20)
- Term values within valid range (0 to 2^n - 1)
- Input file format (3 lines)
- Term prefixes (m/M for minterms/maxterms, d for don't cares)

Warnings are displayed for invalid terms, which are then ignored.
Errors cause the program to exit with a non-zero status code.

PROJECT STRUCTURE
-----------------
origin/
├── CMakeLists.txt              # Build configuration
├── README.txt                  # This file
├── build/                      # Build output directory
├── include/                    # Header files
│   ├── expression.h
│   ├── implicant.h
│   ├── qm-minimizer.h
│   └── verilog-generator.h
├── src/                        # Source files
│   ├── main.cpp
│   ├── expression.cpp
│   ├── implicant.cpp
│   ├── qm-minimizer.cpp
│   └── verilog-generator.cpp
├── utils/                      # Utility files
│   ├── enums.utils.h
│   ├── verilog_utils.h
│   └── verilog_utils.cpp
└── testing/
    └── data/                   # Test input files
        ├── test1.txt
        ├── test2.txt
        └── ...

KEY ALGORITHMS
--------------
1. Implicant Grouping: Groups minterms by number of 1's
2. Iterative Combination: Combines adjacent groups differing by 1 bit
3. Prime Implicant Identification: Identifies uncombined implicants
4. Essential PI Detection: Finds PIs that uniquely cover minterms
5. PI Table Solving: Greedy covering algorithm (heuristic)
6. Verilog Generation: Converts minimized expression to structural Verilog

DATA STRUCTURES
---------------
- Expression: Stores number of variables, minterms, and don't cares
- Implicant: Represents product term with binary representation and coverage
- QMMinimizer: Implements the Quine-McCluskey algorithm
- VerilogGenerator: Converts minimized expressions to Verilog code

KNOWN LIMITATIONS
-----------------
- The PI table solving uses a heuristic (greedy) approach rather than
  Petrick's method, so it may not always find ALL minimal solutions
- Very large functions (>10 variables) may take significant time
- The program requires exact file format compliance

TEAM CONTRIBUTIONS
------------------
This project was completed by a team of three members:

1. Ahmed Saad:
   - Initiated the class structures and basic code framework
   - Implemented Implicant class with all operator overloads
   - Implemented QMMinimizer constructor with implicant grouping
   - Implemented Petrick's method (heuristic covering algorithm)
   - Designed the overall data structure architecture

2. Mahmoud Alaskandrani:
   - Implemented combine() and combine_helper() functions
   - Developed VerilogGenerator class for code generation
   - Implemented Verilog utility functions (escaping, formatting)
   - Assisted in debugging and testing all components
   - Fixed compilation issues and integrated components

3. Amonios:
   - Implemented main.cpp driver program
   - Developed command-line interface and file I/O handling
   - Implemented minimize() function integrating all components
   - Created test cases and validated program output
   - Coordinated integration of all modules

All team members contributed to documentation, testing, and debugging.

CREDITS
-------
Developed for CSCE2301 - Digital Design I, Fall 2025
American University in Cairo

===============================================================================

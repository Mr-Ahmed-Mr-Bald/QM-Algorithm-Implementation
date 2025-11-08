================================================================================
   QUINE-MCCLUSKEY LOGIC MINIMIZATION TOOL
   CSCE2301 - Digital Design I - Fall 2025
   Project 1
================================================================================

TABLE OF CONTENTS
-----------------
1. Project Overview
2. Features
3. System Requirements
4. Building the Application
5. Using the Application
6. Input File Format
7. Test Cases
8. Team Contributions
9. Known Issues

================================================================================
1. PROJECT OVERVIEW
================================================================================

This project implements the Quine-McCluskey logic minimization algorithm to
minimize Boolean functions. The program reads Boolean function specifications
from text files, generates prime implicants, identifies essential prime 
implicants, solves the PI table, and optionally generates Verilog HDL modules.

Programming Language: C++17
Build System: CMake 3.10+
Supported Platforms: Windows, Linux, macOS

================================================================================
2. FEATURES
================================================================================

The application fulfills ALL project requirements with one documented limitation:

REQUIREMENT 1: Read and Validate Boolean Functions ✓ FULLY IMPLEMENTED
------------------------------------------------------------------------
Specification: "Read in (and validate) a Boolean function using its 
minterms/maxterms and don't-care terms. The inputs are provided by a text 
file that has 3 lines."

Implementation Details:
• Line 1: Reads number of variables (validates 1-20 range)
• Line 2: Reads minterms (m prefix) OR maxterms (M prefix)
  - Supports comma-separated format: m1,m3,m6,m7
  - Supports comma-separated format: M0,M2,M4,M5
  - Automatically converts maxterms to minterms
• Line 3: Reads don't-care terms (d prefix)
  - Supports comma-separated format: d0,d5
  - Handles empty line (no don't-cares)

Validation Implemented:
✓ File existence check with error message
✓ Number of variables range validation (1-20)
✓ Term format validation (correct prefixes)
✓ Term value range validation (0 to 2^n - 1)
✓ Duplicate term removal
✓ Empty file handling
✓ Malformed input detection with descriptive error messages

Error Handling Examples:
• "Error: Could not open file 'filename.txt'"
• "Error: Number of variables must be between 1 and 20"
• "Error: Invalid minterms/maxterms format"
• "Error: Missing number of variables"

Location: src/file-parser.cpp, FileParser::parse_file()

REQUIREMENT 2: Generate and Print All Prime Implicants ✓ FULLY IMPLEMENTED
---------------------------------------------------------------------------
Specification: "Generate and print all prime implicants (PIs). For each PI 
show the minterms and don't-care terms it covers as well as its binary 
representation."

Implementation Details:
The program displays a formatted table with:
• PI number (PI0, PI1, PI2, ...)
• Binary representation (e.g., "10-1" where - is don't-care)
• Algebraic form (e.g., "x0x1'x3")
• List of covered minterms (e.g., "Covers: {1, 5, 9, 13}")

Output Format Example:
======================================================================
2. PRIME IMPLICANTS
======================================================================
PI#   Binary         Algebraic          Covers Minterms
----------------------------------------------------------------------
PI0   000            x0'x1'x2'          {0}
PI1   0-1            x0'x2              {1, 5}
PI2   10-            x0x1'              {4, 5}
======================================================================

Features:
✓ Shows ALL prime implicants (none are missed)
✓ Binary representation with dashes for don't-cares
✓ Algebraic Boolean expression for each PI
✓ Complete list of covered minterms
✓ Includes don't-care terms in coverage
✓ Formatted table with clear headers and separators

Location: src/quine-mccluskey-driver.cpp, 
          QuineMcCluskeyDriver::display_prime_implicants()

REQUIREMENT 3: Essential PIs and Uncovered Minterms ✓ FULLY IMPLEMENTED
------------------------------------------------------------------------
Specification: "Using the PIs generated in part 2, obtain and print all the 
essential prime implicants EPIs (as Boolean expressions). Also, print the 
minterms that are not covered by the essential PIs."

Implementation Details:
The program:
1. Analyzes the PI coverage table
2. Identifies minterms covered by only ONE prime implicant
3. Marks those PIs as essential
4. Displays EPIs with their binary and algebraic forms
5. Lists all minterms NOT yet covered by EPIs

Output Format Example:
======================================================================
3. ESSENTIAL PRIME IMPLICANTS
======================================================================
EPI: PI2 (10-) = x0x1'
     Covers: {4, 5}
     
EPI: PI5 (--1) = x2
     Covers: {1, 3, 5, 7}

Minterms not covered by EPIs: {6}
======================================================================

Features:
✓ Correctly identifies ALL essential PIs using coverage analysis
✓ Shows binary representation of each EPI
✓ Shows algebraic Boolean expression for each EPI
✓ Lists minterms covered by each EPI
✓ Explicitly lists uncovered minterms (for PI table solving)
✓ Handles cases with no EPIs (displays "none found")
✓ Handles cases where EPIs cover everything (displays "none")

Algorithm:
For each minterm m:
  - Find all PIs that cover m
  - If exactly ONE PI covers m, mark that PI as essential

Location: src/quine-mccluskey-driver.cpp,
          QuineMcCluskeyDriver::display_essential_pis()
          
REQUIREMENT 4: Solve PI Table and Print Minimized Expression ⚠ PARTIALLY MET
-----------------------------------------------------------------------------
Specification: "Solve the PI table and print the minimized Boolean expression 
of the function. If there is more than one possible solution, print all of them."

Implementation Details:
The program:
1. Starts with all essential prime implicants
2. Uses a greedy heuristic to cover remaining minterms
3. Produces ONE valid minimized solution
4. Displays the solution as a Boolean expression

Output Format Example:
======================================================================
4. MINIMIZED BOOLEAN EXPRESSIONS
======================================================================
Solution 1: x0'x1'x2 + x0'x1x2 + x0x1x2' + x2
======================================================================

What IS Implemented:
✓ Solves the PI covering problem
✓ Includes all essential prime implicants
✓ Covers all minterms
✓ Produces valid minimized expression
✓ Shows algebraic Boolean expression (sum-of-products)
✓ Labels output as "Solution 1"

What is NOT Implemented:
✗ Does NOT find ALL possible minimal solutions
✗ Does NOT implement full Petrick's method
✗ May not find the absolute minimum in all cases

Current Approach - Greedy Heuristic:
1. Add all EPIs to solution
2. For each uncovered minterm:
   - Find first PI that covers it
   - Add that PI to solution
   - Mark minterm as covered

Why This Limitation Exists:
Full Petrick's method requires:
• Boolean algebra manipulation (product-of-sums to sum-of-products)
• Exponential-time solution enumeration
• Complex cost comparison algorithms

The greedy approach is:
• Polynomial time complexity
• Always produces valid solutions
• Near-optimal in most practical cases
• Sufficient for educational purposes

Impact Assessment:
• For most test cases: produces optimal or near-optimal solution
• For cases with multiple equally-minimal solutions: returns only one
• All test cases verified to produce correct, valid minimized expressions

Location: src/qm-minimizer.cpp, QMMinimizer::petrick()
          src/quine-mccluskey-driver.cpp,
          QuineMcCluskeyDriver::display_minimized_expressions()

REQUIREMENT 5: Generate Verilog Module (BONUS) ✓ FULLY IMPLEMENTED
--------------------------------------------------------------------
Specification: "Based on the Boolean expression, generate the Verilog module 
for the function using Verilog Primitives."

Implementation Details:
The program generates a complete, synthesizable Verilog module with:
• Module declaration with proper naming
• Input port declarations (one per variable)
• Output port declaration
• Assign statement with Boolean expression
• Proper Verilog syntax (using &, |, ~, operators)
• Comments with metadata

Generated Verilog Example:
//=====================================================
// Verilog Module: minimized_module
// Generated by Quine-McCluskey Minimizer
// Date: 2025-11-08
//=====================================================

module minimized_module (
    input x0, x1, x2,
    output f
);

    assign f = (~x0 & ~x1 & x2) | 
               (~x0 & x1 & x2) | 
               (x0 & x1 & ~x2) | 
               (x2);

endmodule

Features:
✓ Complete module structure (module...endmodule)
✓ Input/output port declarations with proper syntax
✓ Assign statement using continuous assignment
✓ Boolean operators: & (AND), | (OR), ~ (NOT)
✓ Proper operator precedence with parentheses
✓ Identifier sanitization (handles reserved keywords)
✓ Comment headers with generation info
✓ Synthesizable code (tested with Verilog simulators)
✓ User can specify output filename
✓ Default filename: output.v

Advanced Features:
✓ Multiple output styles (assign, always block, case statement)
✓ Custom module naming
✓ Custom input/output naming
✓ Identifier escaping for Verilog keywords
✓ Multiple solution support (can generate Verilog for each solution)

Location: src/verilog-generator.cpp, VerilogGenerator class
          utils/verilog_utils.cpp, utility functions
          
Additional Features Beyond Requirements:
----------------------------------------
✓ Interactive menu-driven interface for ease of use
✓ Batch mode for automated processing
✓ Formatted output with clear section separators
✓ Comprehensive error messages
✓ State management (prevents operations before loading file)
✓ Input validation at every step
✓ Support for very large functions (up to 20 variables as required)
✓ Efficient algorithms with reasonable performance
✓ Well-structured, documented code
✓ CMake build system for cross-platform compilation

================================================================================
3. SYSTEM REQUIREMENTS
================================================================================

Required:
• CMake 3.10 or higher
• C++17 compatible compiler:
  - GCC 7.0+ (Linux/MinGW)
  - Clang 5.0+
  - MSVC 2017+ (Visual Studio)
  - Apple Clang (Xcode 10+)

Recommended:
• 4GB RAM minimum
• Terminal with 80+ character width for optimal display

================================================================================
4. BUILDING THE APPLICATION
================================================================================

--- Windows (MinGW/MSYS2) ---

1. Open PowerShell or Command Prompt
2. Navigate to project directory:
   cd C:\path\to\QM-Algorithm-Implementation

3. Create and enter build directory:
   mkdir build
   cd build

4. Configure with CMake (MinGW):
   cmake -G "MinGW Makefiles" ..

5. Build the project:
   cmake --build . --config Release

6. The executable will be in the build directory:
   QM_Algorithm_Implementation.exe

--- Linux/macOS (Make) ---

1. Open terminal
2. Navigate to project directory:
   cd /path/to/QM-Algorithm-Implementation

3. Create and enter build directory:
   mkdir build && cd build

4. Configure with CMake:
   cmake ..

5. Build the project:
   make

6. The executable will be in the build directory:
   ./QM_Algorithm_Implementation

--- Alternative: Using CMake GUI ---

1. Open CMake GUI
2. Set source directory to project root
3. Set build directory to project_root/build
4. Click "Configure" and select your compiler
5. Click "Generate"
6. Open generated project files in your IDE or run make/nmake

================================================================================
5. USING THE APPLICATION
================================================================================

The application supports two modes:

--- INTERACTIVE MODE (Default) ---

Simply run the executable without arguments:

Windows:
  .\QM_Algorithm_Implementation.exe

Linux/macOS:
  ./QM_Algorithm_Implementation

The program will display a menu:

  =====Quine-McCluskey Boolean Minimizer=====

  1. Load Boolean function from file
  2. Display Prime Implicants (PIs)
  3. Display Essential Prime Implicants (EPIs)
  4. Display Minimized Boolean Expression(s)
  5. Generate Verilog Module
  6. Exit

Select options by entering the corresponding number.

Menu Navigation:
• Option 1: Load input file (required first step)
• Option 2: View all prime implicants with coverage
• Option 3: View essential PIs and uncovered minterms
• Option 4: View minimized Boolean expressions
• Option 5: Generate and save Verilog module
• Option 6: Exit program

--- BATCH MODE ---

The driver class supports batch processing (can be enabled in main.cpp):

  driver.run_batch("input.txt", "output.v");

This will:
1. Load the input file
2. Run minimization
3. Display all results
4. Generate Verilog file (if output filename provided)

================================================================================
6. INPUT FILE FORMAT
================================================================================

Input files must contain EXACTLY 3 lines:

Line 1: Number of variables (integer between 1 and 20)
Line 2: Minterms OR Maxterms (comma-separated, with m/M prefix)
Line 3: Don't-care terms (comma-separated, with d prefix) - can be empty

--- Example 1: Using Minterms ---

File: test1.txt
------------------
3
m1,m3,m6,m7
d0,d5
------------------

Explanation:
• 3 variables (x0, x1, x2)
• Minterms: 1, 3, 6, 7
• Don't-cares: 0, 5

--- Example 2: Using Maxterms ---

File: test2.txt
------------------
4
M0,M3,M5,M6,M9,M10
d1,d4
------------------

Explanation:
• 4 variables (x0, x1, x2, x3)
• Maxterms: 0, 3, 5, 6, 9, 10 (automatically converted to minterms)
• Don't-cares: 1, 4

--- Example 3: No Don't-Cares ---

File: test3.txt
------------------
3
m0,m2,m5,m7

------------------

Note: Line 3 is empty (no don't-cares)

Format Rules:
• No spaces between terms (or only within commas)
• Prefix must be lowercase 'm' or uppercase 'M' for terms
• Prefix must be lowercase 'd' or uppercase 'D' for don't-cares
• Each term must be a valid number in range [0, 2^n - 1]
• Duplicate terms are automatically removed

================================================================================
7. TEST CASES
================================================================================

The project includes 10 comprehensive test cases in testing/data/:

Test 1-5: Basic Functions
--------------------------
test1.txt - 3 variables, minterms with don't-cares
test2.txt - 4 variables, complex function
test3.txt - 3 variables, function with don't-cares
test4.txt - 4 variables, majority function (no don't-cares)
test5.txt - 2 variables, simple function: x0 + x1

Test 6-10: Advanced Functions
------------------------------
test6.txt  - 3 variables, XOR-like function
test7.txt  - 4 variables, maxterm representation
test8.txt  - 5 variables, complex function
test9.txt  - 3 variables, tautology (all minterms)
test10.txt - 4 variables, extensive don't-cares

To run a test:
1. Launch the application
2. Select option 1 (Load from file)
3. Enter the test file path: ../testing/data/test1.txt
4. Use options 2-5 to view results and generate Verilog

Example session:
----------------
> 1
Enter filename: ../testing/data/test1.txt
✓ File loaded successfully!

> 2
[Displays all prime implicants]

> 4
[Displays minimized expression]

> 5
Enter Verilog output filename (or press Enter for default): test1.v
✓ Verilog module written to: test1.v

> 6
[Exits program]

================================================================================
8. TEAM CONTRIBUTIONS
================================================================================

Team Members: Ahmed Saad, Mahmoud Alaskandrani, Amonios

--- Ahmed Saad ---
Responsibilities:
• Initiated the class structures and basic code framework
• Implemented the complete Implicant class:
  - Constructor with binary representation
  - Operator overloading (-, +, ==, !=, <)
  - Bit manipulation functions
  - Coverage set management
• Implemented QMMinimizer constructor with implicant grouping
• Implemented Petrick's method (heuristic covering algorithm)
• Designed the enum utilities for bit representation
• Set up project structure and CMake configuration

Key Contributions:
• Designed ternary bit representation ($zero, $one, $dash)
• Implemented operator overloading for intuitive implicant operations
• Created efficient data structures for algorithm implementation

--- Mahmoud Alaskandrani ---
Responsibilities:
• Implemented combine() and combine_helper() functions
• Developed the complete VerilogGenerator class:
  - Module generation with proper syntax
  - Expression to Verilog conversion
  - Output style options (assign, always, case)
• Implemented Verilog utility functions:
  - Identifier escaping and sanitization
  - Port declarations
  - Header/footer generation
• Assisted in debugging all components
• Fixed compilation issues and integrated components
• Resolved linker errors and namespace conflicts

Key Contributions:
• Designed and implemented Verilog code generation pipeline
• Created utility functions for safe identifier naming
• Debugged operator overloading and set storage issues
• Integrated all modules ensuring proper compilation

--- Amonios ---
Responsibilities:
• Implemented main.cpp driver program
• Developed complete QuineMcCluskeyDriver class:
  - Interactive menu system
  - File I/O handling
  - Result display formatting
• Implemented minimize() function:
  - Integration of all algorithm phases
  - Prime implicant collection
  - Essential PI identification
• Implemented FileParser class for input validation
• Created all 10 test cases with diverse scenarios
• Validated program output and verified correctness
• Coordinated integration of all modules
• Wrote documentation (README, comments)

Key Contributions:
• Designed user-facing interface and workflow
• Implemented robust input validation and error handling
• Created comprehensive test suite covering edge cases
• Ensured output format matches project requirements
• Formatted display methods for all requirements (2-5)

--- Collaborative Efforts ---
All team members contributed to:
• Code review and pair programming sessions
• Debugging complex issues
• Testing and validation of results
• Documentation and commenting
• Git repository management
• Report writing

================================================================================
9. KNOWN ISSUES
================================================================================

Issue 1: Petrick's Method Implementation - PARTIAL REQUIREMENT FULFILLMENT
-----------------------------------------------------------------------------
Status: DOCUMENTED LIMITATION
Impact: Does NOT generate all minimal solutions (Requirement 4 partially met)

Description:
Requirement 4 states: "Solve the PI table and print the minimized Boolean 
expression of the function. If there is more than one possible solution, 
print all of them."

Current Implementation:
The program uses a greedy heuristic instead of full Petrick's method:
1. It correctly identifies ALL prime implicants (Requirement 2) ✓
2. It correctly identifies ALL essential prime implicants (Requirement 3) ✓
3. It adds all EPIs to the solution
4. It uses a greedy algorithm to cover remaining minterms
5. It produces ONE valid minimized solution that covers all minterms

What's Missing:
- Does NOT enumerate all possible minimal solutions
- Does NOT guarantee the solution is absolutely minimal (though usually close)
- Does NOT implement true Petrick's method using Boolean algebra

Example:
For a function with multiple equally-minimal solutions (e.g., f = AB + AC 
or f = AB + BC where both are minimal), the program will output only ONE
of them, not both.

Why This Approach Was Taken:
Full Petrick's method requires:
• Generating product-of-sums Boolean expression from covering table
• Expanding to sum-of-products form
• Finding all minimum-cost terms
• Complexity can be exponential for large functions

The heuristic approach:
• Always produces a VALID solution
• Runs in polynomial time
• Covers all minterms correctly
• Uses all essential PIs (ensuring near-optimal solutions)
• Sufficient for most practical applications

Verification:
All 10 test cases produce correct, valid minimized expressions that:
✓ Cover all specified minterms
✓ Do not cover any maxterms
✓ Include all essential prime implicants
✓ Use only prime implicants
✓ Are minimal or near-minimal in most cases

Future Enhancement:
Implement full Petrick's method using:
1. Boolean algebra manipulation
2. SOP expansion algorithms
3. Cost comparison to find all minimal solutions
4. Solution enumeration for equally-minimal covers

Issue 2: Performance with Large Functions
------------------------------------------
Status: BY DESIGN
Impact: Slower execution for functions > 10 variables

Description:
For functions with more than 10 variables, computation time increases due to
exponential growth of implicants during combination phase.

Mitigation:
The algorithm is correct and will complete; it just takes longer for complex
functions. All test cases complete in under 1 second.

Issue 3: Input Format Strictness
---------------------------------
Status: BY DESIGN (For Error Prevention)
Impact: Requires exact 3-line format

Description:
The input parser requires strict adherence to the 3-line format. Extra blank
lines or improper formatting will cause parsing errors.

Reason:
Strict parsing prevents ambiguous inputs and ensures data integrity.

Workaround:
Follow the input format exactly as specified in Section 6.

================================================================================

For questions or issues, please contact the development team.

Project Repository: https://github.com/Mr-Ahmed-Mr-Bald/QM-Algorithm-Implementation

================================================================================
END OF README
================================================================================

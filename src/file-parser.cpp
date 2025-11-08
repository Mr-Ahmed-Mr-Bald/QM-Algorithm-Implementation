#include "../include/file-parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <algorithm>

using namespace std;

bool FileParser::parse_file(const string& filename, Expression& expr) {
    ifstream infile(filename);
    
    if (!infile.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'\n";
        return false;
    }
    
    string line;
    
    // Line 1: Number of variables
    if (!getline(infile, line)) {
        cerr << "Error: Missing number of variables\n";
        return false;
    }
    
    try {
        expr.numberOfBits = stoi(line);
        if (expr.numberOfBits <= 0 || expr.numberOfBits > 20) {
            cerr << "Error: Number of variables must be between 1 and 20\n";
            return false;
        }
    } catch (...) {
        cerr << "Error: Invalid number of variables\n";
        return false;
    }
    
    // Line 2: Minterms or Maxterms (m1,m3,m6,m7 or M0,M2,M4,M5)
    if (!getline(infile, line)) {
        cerr << "Error: Missing minterms/maxterms line\n";
        return false;
    }
    
    bool is_maxterm = false;
    if (!parse_terms_line(line, expr.minterms, is_maxterm)) {
        cerr << "Error: Invalid minterms/maxterms format\n";
        return false;
    }
    
    // Convert maxterms to minterms if needed
    if (is_maxterm) {
        vector<int> maxterms = expr.minterms;
        expr.minterms.clear();
        int max_val = (1 << expr.numberOfBits);
        for (int i = 0; i < max_val; i++) {
            if (find(maxterms.begin(), maxterms.end(), i) == maxterms.end()) {
                expr.minterms.push_back(i);
            }
        }
    }
    
    // Line 3: Don't cares (d0,d5) - optional
    expr.dontcares.clear();
    if (getline(infile, line) && !line.empty()) {
        parse_dontcares_line(line, expr.dontcares);
    }
    
    infile.close();
    return true;
}

bool FileParser::parse_terms_line(const string& line, vector<int>& terms, bool& is_maxterm) {
    terms.clear();
    
    // Remove whitespace
    string cleaned;
    for (char c : line) {
        if (!isspace(c)) cleaned += c;
    }
    
    if (cleaned.empty()) return false;
    
    // Check if minterm (m) or maxterm (M)
    if (cleaned[0] == 'm' || cleaned[0] == 'M') {
        is_maxterm = (cleaned[0] == 'M');
    } else {
        return false;
    }
    
    // Parse: m1,m3,m6,m7
    stringstream ss(cleaned);
    char prefix;
    int num;
    char comma;
    
    while (ss >> prefix) {
        if (prefix != 'm' && prefix != 'M') return false;
        if (!(ss >> num)) return false;
        terms.push_back(num);
        
        // Check for comma
        if (ss.peek() == ',') {
            ss >> comma;
        }
    }
    
    return !terms.empty();
}

bool FileParser::parse_dontcares_line(const string& line, vector<int>& dontcares) {
    dontcares.clear();
    
    // Remove whitespace
    string cleaned;
    for (char c : line) {
        if (!isspace(c)) cleaned += c;
    }
    
    if (cleaned.empty()) return true; // Empty is valid
    
    // Parse: d0,d5
    stringstream ss(cleaned);
    char prefix;
    int num;
    char comma;
    
    while (ss >> prefix) {
        if (prefix != 'd' && prefix != 'D') return false;
        if (!(ss >> num)) return false;
        dontcares.push_back(num);
        
        if (ss.peek() == ',') {
            ss >> comma;
        }
    }
    
    return true;
}
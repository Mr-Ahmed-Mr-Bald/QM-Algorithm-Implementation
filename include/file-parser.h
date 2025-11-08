#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <string>
#include "expression.h"

using std::string;

class FileParser {
public:
    static bool parse_file(const string& filename, Expression& expr);
    
private:
    static bool parse_terms_line(const string& line, vector<int>& terms, bool& is_maxterm);
    static bool parse_dontcares_line(const string& line, vector<int>& dontcares);
};

#endif // FILE_PARSER_H
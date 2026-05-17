#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;


enum TokenType {
    IDENTIFIER,
    INTEGER,
    STRING,
    OPERATOR,
    KEYWORD,
    PUNCTUATION,
    END_OF_FILE,
    ERROR
};



struct Token {
    TokenType type;   // the category of this token
    string    value;  // the actual text of this token
    int       line;
    int       column;

    // Default constructor — creates an EOF token
    Token() : type(END_OF_FILE), value(""), line(0), column(0) {}
    // Constructor with type and value
    Token(TokenType t, string v) : type(t), value(v), line(0), column(0) {}
    Token(TokenType t, string v, int ln, int col)
        : type(t), value(v), line(ln), column(col) {}
};

#endif

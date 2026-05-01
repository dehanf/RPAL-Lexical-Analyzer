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

    // Default constructor — creates an EOF token
    Token() : type(END_OF_FILE), value("") {}
    // Constructor with type and value
    Token(TokenType t, string v) : type(t), value(v) {}
};

#endif

#include "lexical.h"

#include <cctype>
#include <fstream>
#include <set>
#include <stdexcept>

using namespace std;

namespace {
bool isOperatorSymbol(char c) {
    static const string symbols = "+-*<>&.@/:=~|$!#%^_[]{}\"`?";
    return symbols.find(c) != string::npos;
}

bool isPunctuation(char c) {
    return c == '(' || c == ')' || c == ';' || c == ',';
}

bool isKeyword(const string& value) {
    static const set<string> keywords = {
        "let", "where", "true", "false", "not", "fn", "Is", "gr", "ge",
        "ls", "aug", "le", "nil", "dummy", "or", "in", "eq", "ne",
        "and", "rec", "within"
    };
    return keywords.find(value) != keywords.end();
}
}

string tokenTypeToString(TokenType type) {
    switch (type) {
        case IDENTIFIER: return "IDENTIFIER";
        case INTEGER: return "INTEGER";
        case STRING: return "STRING";
        case OPERATOR: return "OPERATOR";
        case KEYWORD: return "KEYWORD";
        case PUNCTUATION: return "PUNCTUATION";
        case END_OF_FILE: return "END_OF_FILE";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

vector<Token> tokenize(const string& fileName) {
    ifstream infile(fileName);
    if (!infile) {
        throw runtime_error("unable to open input file: " + fileName);
    }

    vector<Token> tokens;
    char c;
    int line = 1;
    int column = 0;

    while (infile.get(c)) {
        column++;

        if (isspace(static_cast<unsigned char>(c))) {
            if (c == '\n') {
                line++;
                column = 0;
            }
            continue;
        }

        if (isalpha(static_cast<unsigned char>(c))) {
            int startColumn = column;
            string value(1, c);
            while (infile.peek() != EOF) {
                char next = static_cast<char>(infile.peek());
                if (!isalnum(static_cast<unsigned char>(next)) && next != '_') {
                    break;
                }
                infile.get(next);
                column++;
                value += next;
            }
            tokens.push_back(Token(isKeyword(value) ? KEYWORD : IDENTIFIER,
                                   value, line, startColumn));
            continue;
        }

        if (isdigit(static_cast<unsigned char>(c))) {
            int startColumn = column;
            string value(1, c);
            while (infile.peek() != EOF) {
                char next = static_cast<char>(infile.peek());
                if (!isdigit(static_cast<unsigned char>(next))) {
                    break;
                }
                infile.get(next);
                column++;
                value += next;
            }
            tokens.push_back(Token(INTEGER, value, line, startColumn));
            continue;
        }

        if (c == '\'') {
            int startColumn = column;
            string value(1, c);
            bool closed = false;

            while (infile.get(c)) {
                column++;
                value += c;

                if (c == '\\') {
                    if (infile.get(c)) {
                        column++;
                        value += c;
                    }
                    continue;
                }

                if (c == '\'') {
                    closed = true;
                    break;
                }

                if (c == '\n') {
                    line++;
                    column = 0;
                }
            }

            tokens.push_back(Token(closed ? STRING : ERROR, value, line, startColumn));
            continue;
        }

        if (c == '/' && infile.peek() == '/') {
            while (infile.get(c)) {
                if (c == '\n') {
                    line++;
                    column = 0;
                    break;
                }
                column++;
            }
            continue;
        }

        if (isOperatorSymbol(c)) {
            int startColumn = column;
            string value(1, c);
            while (infile.peek() != EOF) {
                char next = static_cast<char>(infile.peek());
                if (!isOperatorSymbol(next)) {
                    break;
                }
                if (value == "/" && next == '/') {
                    break;
                }
                infile.get(next);
                column++;
                value += next;
            }
            tokens.push_back(Token(OPERATOR, value, line, startColumn));
            continue;
        }

        if (isPunctuation(c)) {
            tokens.push_back(Token(PUNCTUATION, string(1, c), line, column));
            continue;
        }

        tokens.push_back(Token(ERROR, string(1, c), line, column));
    }

    return tokens;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include "Token.h"
using namespace std;

enum STATE { STATE_START, STATE_IDENTIFIER, STATE_COMMENT, STATE_INTEGER, STATE_OPERATOR, STATE_PUNCTUATION, STATE_ERROR };

string tokenTypeToString(TokenType type) {
    switch(type) {
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

int main(int argc, char* argv[]) {
    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    STATE state = STATE_START;
    string s;
    Token token;
    vector<Token> tokens;
    set<string> keywords = {
        "let", "where", "true", "false", "not", "fn", "Is", "gr", "ge", 
        "aug", "le", "nil", "dummy", "or", "in", "eq", "ne", "and", "rec", "within"
    };

    while (getline(infile, s)) {
        s += '\n'; 
        
        for (char c : s) {
            if (state == STATE_START) {
                if (isspace(c)) {
                    continue;
                } else if (isalpha(c)) {
                    state = STATE_IDENTIFIER;
                    token.value = c;
                } else if (isdigit(c)) {
                    state = STATE_INTEGER;
                    token.value = c;
                } else if (string("+-*<>&.|:!#%^@$").find(c) != string::npos) {
                    state = STATE_OPERATOR;
                    token.value = c;
                } else if (string("();,").find(c) != string::npos) {
                    state = STATE_PUNCTUATION;
                    token.type = PUNCTUATION;
                    token.value = c;
                    tokens.push_back(token);
                    token.value.clear();
                    state = STATE_START;
                } else {
                    state = STATE_ERROR;
                    token.type = ERROR;
                    token.value = c;
                    tokens.push_back(token);
                    token.value.clear();
                    state = STATE_START;
                }
            } else if (state == STATE_IDENTIFIER) {
                if (isalnum(c) || c == '_') {
                    token.value += c;
                } else {
                    if (keywords.find(token.value) != keywords.end()) {
                        token.type = KEYWORD;
                    } else {
                        token.type = IDENTIFIER;
                    }
                    tokens.push_back(token);
                    token.value.clear();
                    state = STATE_START;
                    
                    // Reprocess current character
                    if (!isspace(c)) {
                        if (isalpha(c)) {
                            state = STATE_IDENTIFIER;
                            token.value = c;
                        } else if (isdigit(c)) {
                            state = STATE_INTEGER;
                            token.value = c;
                        } else if (string("+-*<>&.|:!#%^@$").find(c) != string::npos) {
                            state = STATE_OPERATOR;
                            token.value = c;
                        } else if (string("();,").find(c) != string::npos) {
                            token.type = PUNCTUATION;
                            token.value = c;
                            tokens.push_back(token);
                            token.value.clear();
                        }
                    }
                }
            } else if (state == STATE_INTEGER) {
                if (isdigit(c)) {
                    token.value += c;
                } else {
                    token.type = INTEGER;
                    tokens.push_back(token);
                    token.value.clear();
                    state = STATE_START;
                    
                    if (!isspace(c)) {
                        if (isalpha(c)) {
                            state = STATE_IDENTIFIER;
                            token.value = c;
                        } else if (string("+-*<>&.|:!#%^@$").find(c) != string::npos) {
                            state = STATE_OPERATOR;
                            token.value = c;
                        } else if (string("();,").find(c) != string::npos) {
                            token.type = PUNCTUATION;
                            token.value = c;
                            tokens.push_back(token);
                            token.value.clear();
                        }
                    }
                }
            } else if (state == STATE_OPERATOR) {
                if (string("+-*<>&.|:!#%^@$").find(c) != string::npos) {
                    token.value += c;
                } else {
                    token.type = OPERATOR;
                    tokens.push_back(token);
                    token.value.clear();
                    state = STATE_START;
                    
                    if (!isspace(c)) {
                        if (isalpha(c)) {
                            state = STATE_IDENTIFIER;
                            token.value = c;
                        } else if (isdigit(c)) {
                            state = STATE_INTEGER;
                            token.value = c;
                        } else if (string("();,").find(c) != string::npos) {
                            token.type = PUNCTUATION;
                            token.value = c;
                            tokens.push_back(token);
                            token.value.clear();
                        }
                    }
                }
            }
        }
    }

    for (const Token& t : tokens) {
        cout << "<" << tokenTypeToString(t.type) << ", \"" << t.value << "\">" << endl;
    }

    infile.close();
    return 0;
}
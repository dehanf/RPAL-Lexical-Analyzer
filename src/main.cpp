#include "ASTNode.h"
#include "CSEMachine.h"
#include "RuntimeValue.h"
#include "Standardizer.h"
#include "lexical.h"
#include "parser.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace {
void printUsageOrError() {
    cerr << "Usage: ./rpal20 [-tokens|-ast|-st] file_name" << endl;
}

bool hasLexicalError(const vector<Token>& tokens) {
    for (const Token& token : tokens) {
        if (token.type == ERROR) {
            cerr << "Lexical Error: invalid token '" << token.value << "'";
            if (token.line > 0) {
                cerr << " at line " << token.line << ", column " << token.column;
            }
            cerr << endl;
            return true;
        }
    }
    return false;
}

void printTokens(const vector<Token>& tokens) {
    for (const Token& token : tokens) {
        cout << "<" << tokenTypeToString(token.type) << ", \""
             << token.value << "\">" << endl;
    }
}
}

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        printUsageOrError();
        return 1;
    }

    string mode;
    string fileName;

    if (argc == 2) {
        fileName = argv[1];
    } else {
        mode = argv[1];
        fileName = argv[2];
        if (mode != "-tokens" && mode != "-ast" && mode != "-st") {
            printUsageOrError();
            return 1;
        }
    }

    try {
        vector<Token> tokens = tokenize(fileName);
        if (hasLexicalError(tokens)) {
            return 1;
        }

        if (mode == "-tokens") {
            printTokens(tokens);
            return 0;
        }

        Parser parser(tokens);
        ASTNode* root = parser.parse();
        ASTNode* standardizedRoot = nullptr;

        if (mode == "-ast") {
            printAST(root);
        } else if (mode == "-st") {
            standardizedRoot = standardize(root);
            printAST(standardizedRoot);
        } else {
            standardizedRoot = standardize(root);
            CSEMachine machine;
            machine.evaluate(standardizedRoot);
        }

        freeTree(standardizedRoot);
        freeTree(root);
        return 0;
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 1;
    }
}

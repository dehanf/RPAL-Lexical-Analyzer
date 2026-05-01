#include "parser.h"
#include <iostream>
using namespace std;

// constructor which takes the list of tokens  from the lexer and initializes the pos to 0 
Parser::Parser(const vector<Token>& toks) : tokens(toks), pos(0) {}


// function that returns a reference to the current token 
// returns EOF token if the list is exhausted
Token& Parser::current() {
    // A static EOF token returned when the list is exhausted
    static Token eof(END_OF_FILE, "$EOF$");

    // If pos is still inside the list, return that token
    if (pos < (int)tokens.size())
        return tokens[pos];

    // Otherwise return the EOF 
    return eof;
}



// Lookahead function which peaks to the token (offset is how many positions to lookahead)
Token& Parser::lookahead(int offset) {
    static Token eof(END_OF_FILE, "$EOF$");

    int idx = pos + offset;                    // calculate the target index
    if (idx < (int)tokens.size())
        return tokens[idx];

    return eof;
}



// check if the tokens are valid in the recursive descent parser 
void Parser::match(const string& expected) {
    if (current().value == expected) {
        pos++;  // token matched — move pos forward
    } else {
        // Token did not match — the program has a syntax error
        cerr << "Syntax Error: expected '" << expected
             << "' but found '"            << current().value
             << "'" << endl;
        exit(1);
    }
}


// building the tree
void Parser::buildTree(const string& label, int n) {
    // Create the new parent node with the given label
    ASTNode* node = new ASTNode(label);

    // Prepare a vector to hold the children in correct order
    // We allocate n slots and fill from back to front
    // because the stack gives us the last child first
    vector<ASTNode*> children(n);
    for (int i = n - 1; i >= 0; i--) {
        children[i] = stack.back();  // take the top of the stack
        stack.pop_back();            // remove it from the stack
    }

    // Attach the children to the parent node
    node->children = children;

    // Push the completed node back onto the stack
    stack.push_back(node);
}



#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "Token.h"
#include "ASTNode.h"
using namespace std;

// ================================================================
//  PARSER CLASS
//
//  What this class does:
//    Takes the list of tokens produced by your lexer and builds
//    an Abstract Syntax Tree (AST) from them.
//
//  How to use it from your main file:
//    vector<Token> tokens = yourLexer.tokenize();
//    Parser parser(tokens);
//    ASTNode* root = parser.parse();
// ================================================================
class Parser {
private:
    vector<Token>    tokens;  // token list received from the lexer
    int              pos;     // index pointing at the current token
    vector<ASTNode*> stack;   // stack used to build AST bottom-up

    // --- Helpers for reading tokens ---
    Token& current();
    Token& lookahead(int offset = 1);
    void   match(const string& expected);

    // --- Helper for building tree nodes ---
    void buildTree(const string& label, int n);

    // --- Helper to check what the current token can start ---
    bool isStartOfRn();

    // --- One function per grammar rule ---
    void E();   // top-level expression
    void Ew();  // where expression
    void T();   // tuple
    void Ta();  // aug
    void Tc();  // conditional
    void B();   // boolean or
    void Bt();  // boolean and
    void Bs();  // boolean not
    void Bp();  // comparison
    void A();   // arithmetic + -
    void At();  // arithmetic * /
    void Af();  // arithmetic **
    void Ap();  // @ infix
    void R();   // function application
    void Rn();  // single value (leaf)
    void D();   // top-level definition
    void Da();  // and definitions
    void Dr();  // rec definition
    void Db();  // definition body
    void Vb();  // variable binding
    void Vl();  // variable list

public:
    Parser(const vector<Token>& tokens);
    ASTNode* parse();
};

#endif

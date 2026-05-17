#include "parser.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;

Parser::Parser(const vector<Token>& toks) : tokens(toks), pos(0) {}

Token& Parser::current() {
    static Token eof(END_OF_FILE, "$EOF$");
    if (pos < static_cast<int>(tokens.size())) {
        return tokens[pos];
    }
    return eof;
}

Token& Parser::lookahead(int offset) {
    static Token eof(END_OF_FILE, "$EOF$");
    int idx = pos + offset;
    if (idx < static_cast<int>(tokens.size())) {
        return tokens[idx];
    }
    return eof;
}

bool Parser::check(const string& value) {
    return current().value == value;
}

void Parser::syntaxError(const string& expected, const Token& found) {
    cerr << "Syntax Error: expected " << expected << " but found '"
         << found.value << "'";
    if (found.line > 0) {
        cerr << " at line " << found.line << ", column " << found.column;
    }
    cerr << endl;
    exit(1);
}

void Parser::internalParserError(const string& message) {
    cerr << "Internal Parser Error: " << message << endl;
    exit(1);
}

void Parser::match(const string& expected) {
    if (check(expected)) {
        pos++;
        return;
    }
    syntaxError("'" + expected + "'", current());
}

void Parser::buildTree(const string& label, int n) {
    if (n < 0 || static_cast<int>(stack.size()) < n) {
        internalParserError("cannot build '" + label + "' with requested child count");
    }

    ASTNode* node = new ASTNode(label);
    vector<ASTNode*> children(n);
    for (int i = n - 1; i >= 0; --i) {
        children[i] = stack.back();
        stack.pop_back();
    }
    node->children = children;
    stack.push_back(node);
}

bool Parser::isStartOfRn() {
    Token& t = current();
    if (t.type == IDENTIFIER || t.type == INTEGER || t.type == STRING) {
        return true;
    }
    if (t.type == KEYWORD &&
        (t.value == "true" || t.value == "false" ||
         t.value == "nil" || t.value == "dummy")) {
        return true;
    }
    return t.type == PUNCTUATION && t.value == "(";
}

void Parser::Rn() {
    Token& t = current();

    if (t.type == IDENTIFIER) {
        stack.push_back(new ASTNode("<ID>", t.value));
        pos++;
    } else if (t.type == INTEGER) {
        stack.push_back(new ASTNode("<INT>", t.value));
        pos++;
    } else if (t.type == STRING) {
        stack.push_back(new ASTNode("<STR>", t.value));
        pos++;
    } else if (t.type == KEYWORD && t.value == "true") {
        stack.push_back(new ASTNode("true"));
        pos++;
    } else if (t.type == KEYWORD && t.value == "false") {
        stack.push_back(new ASTNode("false"));
        pos++;
    } else if (t.type == KEYWORD && t.value == "nil") {
        stack.push_back(new ASTNode("nil"));
        pos++;
    } else if (t.type == KEYWORD && t.value == "dummy") {
        stack.push_back(new ASTNode("dummy"));
        pos++;
    } else if (t.type == PUNCTUATION && t.value == "(") {
        match("(");
        E();
        match(")");
    } else {
        syntaxError("identifier, integer, string, literal, or '('", t);
    }
}

void Parser::R() {
    Rn();
    while (isStartOfRn()) {
        Rn();
        buildTree("gamma", 2);
    }
}

void Parser::Ap() {
    R();
    while (current().type == OPERATOR && current().value == "@") {
        pos++;
        if (current().type != IDENTIFIER) {
            syntaxError("identifier after '@'", current());
        }
        stack.push_back(new ASTNode("<ID>", current().value));
        pos++;
        R();
        buildTree("@", 3);
    }
}

void Parser::Af() {
    Ap();
    if (current().type == OPERATOR && current().value == "**") {
        pos++;
        Af();
        buildTree("**", 2);
    }
}

void Parser::At() {
    Af();
    while (current().type == OPERATOR &&
           (current().value == "*" || current().value == "/")) {
        string op = current().value;
        pos++;
        Af();
        buildTree(op, 2);
    }
}

void Parser::A() {
    if (current().type == OPERATOR && current().value == "+") {
        pos++;
        At();
    } else if (current().type == OPERATOR && current().value == "-") {
        pos++;
        At();
        buildTree("neg", 1);
    } else {
        At();
    }

    while (current().type == OPERATOR &&
           (current().value == "+" || current().value == "-")) {
        string op = current().value;
        pos++;
        At();
        buildTree(op, 2);
    }
}

void Parser::Bp() {
    A();
    Token& t = current();

    if ((t.type == KEYWORD && t.value == "gr") ||
        (t.type == OPERATOR && t.value == ">")) {
        pos++;
        A();
        buildTree("gr", 2);
    } else if ((t.type == KEYWORD && t.value == "ge") ||
               (t.type == OPERATOR && t.value == ">=")) {
        pos++;
        A();
        buildTree("ge", 2);
    } else if ((t.type == KEYWORD && t.value == "ls") ||
               (t.type == OPERATOR && t.value == "<")) {
        pos++;
        A();
        buildTree("ls", 2);
    } else if ((t.type == KEYWORD && t.value == "le") ||
               (t.type == OPERATOR && t.value == "<=")) {
        pos++;
        A();
        buildTree("le", 2);
    } else if (t.type == KEYWORD && t.value == "eq") {
        pos++;
        A();
        buildTree("eq", 2);
    } else if (t.type == KEYWORD && t.value == "ne") {
        pos++;
        A();
        buildTree("ne", 2);
    }
}

void Parser::Bs() {
    if (current().type == KEYWORD && current().value == "not") {
        pos++;
        Bp();
        buildTree("not", 1);
    } else {
        Bp();
    }
}

void Parser::Bt() {
    Bs();
    while (current().type == OPERATOR && current().value == "&") {
        pos++;
        Bs();
        buildTree("&", 2);
    }
}

void Parser::B() {
    Bt();
    while (current().type == KEYWORD && current().value == "or") {
        pos++;
        Bt();
        buildTree("or", 2);
    }
}

void Parser::Tc() {
    B();
    if (current().type == OPERATOR && current().value == "->") {
        pos++;
        Tc();
        match("|");
        Tc();
        buildTree("->", 3);
    }
}

void Parser::Ta() {
    Tc();
    while (current().type == KEYWORD && current().value == "aug") {
        pos++;
        Tc();
        buildTree("aug", 2);
    }
}

void Parser::T() {
    Ta();
    int count = 1;
    while (current().type == PUNCTUATION && current().value == ",") {
        pos++;
        Ta();
        count++;
    }
    if (count > 1) {
        buildTree("tau", count);
    }
}

void Parser::Ew() {
    T();
    if (current().type == KEYWORD && current().value == "where") {
        pos++;
        Dr();
        buildTree("where", 2);
    }
}

void Parser::E() {
    if (current().type == KEYWORD && current().value == "let") {
        pos++;
        D();
        match("in");
        E();
        buildTree("let", 2);
    } else if (current().type == KEYWORD && current().value == "fn") {
        pos++;
        int count = 0;
        if (!(current().type == IDENTIFIER ||
              (current().type == PUNCTUATION && current().value == "("))) {
            syntaxError("one or more variable bindings after 'fn'", current());
        }
        do {
            Vb();
            count++;
        } while (current().type == IDENTIFIER ||
                 (current().type == PUNCTUATION && current().value == "("));
        match(".");
        E();
        buildTree("lambda", count + 1);
    } else {
        Ew();
    }
}

void Parser::Vl() {
    if (current().type != IDENTIFIER) {
        syntaxError("identifier", current());
    }

    stack.push_back(new ASTNode("<ID>", current().value));
    pos++;
    int count = 1;

    while (current().type == PUNCTUATION && current().value == ",") {
        pos++;
        if (current().type != IDENTIFIER) {
            syntaxError("identifier after ','", current());
        }
        stack.push_back(new ASTNode("<ID>", current().value));
        pos++;
        count++;
    }

    if (count > 1) {
        buildTree(",", count);
    }
}

void Parser::Vb() {
    if (current().type == IDENTIFIER) {
        stack.push_back(new ASTNode("<ID>", current().value));
        pos++;
    } else if (current().type == PUNCTUATION && current().value == "(") {
        pos++;
        if (current().type == PUNCTUATION && current().value == ")") {
            pos++;
            buildTree("()", 0);
        } else {
            Vl();
            match(")");
        }
    } else {
        syntaxError("identifier or '('", current());
    }
}

void Parser::Db() {
    if (current().type == PUNCTUATION && current().value == "(") {
        pos++;
        D();
        match(")");
    } else if (current().type == IDENTIFIER) {
        Token& next = lookahead(1);
        bool isFcnForm =
            next.type == IDENTIFIER ||
            (next.type == PUNCTUATION && next.value == "(");

        if (isFcnForm) {
            stack.push_back(new ASTNode("<ID>", current().value));
            pos++;
            int count = 1;

            do {
                Vb();
                count++;
            } while (current().type == IDENTIFIER ||
                     (current().type == PUNCTUATION && current().value == "("));

            match("=");
            E();
            count++;
            buildTree("fcn_form", count);
        } else {
            Vl();
            match("=");
            E();
            buildTree("=", 2);
        }
    } else {
        syntaxError("definition", current());
    }
}

void Parser::Dr() {
    if (current().type == KEYWORD && current().value == "rec") {
        pos++;
        Db();
        buildTree("rec", 1);
    } else {
        Db();
    }
}

void Parser::Da() {
    Dr();
    int count = 1;
    while (current().type == KEYWORD && current().value == "and") {
        pos++;
        Dr();
        count++;
    }
    if (count > 1) {
        buildTree("and", count);
    }
}

void Parser::D() {
    Da();
    if (current().type == KEYWORD && current().value == "within") {
        pos++;
        D();
        buildTree("within", 2);
    }
}

ASTNode* Parser::parse() {
    E();

    if (pos < static_cast<int>(tokens.size())) {
        syntaxError("end of program", current());
    }

    if (stack.size() != 1) {
        stringstream ss;
        ss << "stack has " << stack.size() << " items but expected exactly 1";
        internalParserError(ss.str());
    }

    return stack[0];
}

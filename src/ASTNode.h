#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <vector>
using namespace std;

// ================================================================
//  ASTNODE
//
//  This is one node in the Abstract Syntax Tree.
//
//  Every node has three things:
//
//    label    — what type of node this is
//                 Internal nodes use the grammar label:
//                   "let"  "lambda"  "gamma"  "+"  "fcn_form"
//                 Leaf nodes use:
//                   "<ID>"   for identifiers  e.g. variable x
//                   "<INT>"  for integers     e.g. number 42
//                   "<STR>"  for strings      e.g. 'hello'
//
//    value    — only used for leaf nodes
//                 e.g. if label is "<ID>" then value is "x"
//                 e.g. if label is "<INT>" then value is "42"
//                 internal nodes leave this empty ""
//
//    children — list of child nodes in left-to-right order
//                 leaf nodes have an empty children list
// ================================================================
struct ASTNode {
    string           label;
    string           value;
    vector<ASTNode*> children;

    // Use this constructor for internal nodes
    // e.g.  ASTNode("let")   ASTNode("gamma")   ASTNode("+")
    ASTNode(const string& lbl)
        : label(lbl), value("") {}

    // Use this constructor for leaf nodes
    // e.g.  ASTNode("<ID>", "x")   ASTNode("<INT>", "42")
    ASTNode(const string& lbl, const string& val)
        : label(lbl), value(val) {}

    void addChild(ASTNode* child);
    bool isLeaf() const;
    string displayLabel() const;
};

void printAST(ASTNode* root, int depth = 0);
ASTNode* cloneTree(ASTNode* root);
void freeTree(ASTNode* root);

#endif

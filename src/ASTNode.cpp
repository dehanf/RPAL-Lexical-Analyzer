#include "ASTNode.h"

#include <iostream>

using namespace std;

void ASTNode::addChild(ASTNode* child) {
    children.push_back(child);
}

bool ASTNode::isLeaf() const {
    return children.empty();
}

string ASTNode::displayLabel() const {
    if (value.empty()) {
        return label;
    }
    return label.substr(0, label.size() - 1) + ":" + value + ">";
}

void printAST(ASTNode* root, int depth) {
    if (root == nullptr) {
        return;
    }

    for (int i = 0; i < depth; ++i) {
        cout << ".";
    }
    cout << root->displayLabel() << endl;

    for (ASTNode* child : root->children) {
        printAST(child, depth + 1);
    }
}

ASTNode* cloneTree(ASTNode* root) {
    if (root == nullptr) {
        return nullptr;
    }

    ASTNode* copy = new ASTNode(root->label, root->value);
    for (ASTNode* child : root->children) {
        copy->addChild(cloneTree(child));
    }
    return copy;
}

void freeTree(ASTNode* root) {
    if (root == nullptr) {
        return;
    }

    for (ASTNode* child : root->children) {
        freeTree(child);
    }
    delete root;
}

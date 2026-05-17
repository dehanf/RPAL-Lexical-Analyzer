#include "Standardizer.h"

#include <stdexcept>
#include <vector>

using namespace std;

namespace {
ASTNode* node(const string& label, const vector<ASTNode*>& children = {}) {
    ASTNode* n = new ASTNode(label);
    n->children = children;
    return n;
}

ASTNode* standardizeLet(ASTNode* root) {
    // let X = E in P  ==>  gamma(lambda(X, P), E)
    ASTNode* def = root->children[0];
    ASTNode* body = root->children[1];
    if (def->label != "=" || def->children.size() != 2) {
        throw runtime_error("let standardization expects '=' definition");
    }
    ASTNode* lambda = node("lambda", {def->children[0], body});
    return node("gamma", {lambda, def->children[1]});
}

ASTNode* standardizeWhere(ASTNode* root) {
    // P where X = E  ==>  gamma(lambda(X, P), E)
    ASTNode* body = root->children[0];
    ASTNode* def = root->children[1];
    if (def->label != "=" || def->children.size() != 2) {
        throw runtime_error("where standardization expects '=' definition");
    }
    ASTNode* lambda = node("lambda", {def->children[0], body});
    return node("gamma", {lambda, def->children[1]});
}

ASTNode* standardizeWithin(ASTNode* root) {
    // X1 = E1 within X2 = E2  ==>  X2 = gamma(lambda(X1, E2), E1)
    ASTNode* left = root->children[0];
    ASTNode* right = root->children[1];
    if (left->label != "=" || right->label != "=" ||
        left->children.size() != 2 || right->children.size() != 2) {
        throw runtime_error("within standardization expects two '=' definitions");
    }
    ASTNode* lambda = node("lambda", {left->children[0], right->children[1]});
    ASTNode* gamma = node("gamma", {lambda, left->children[1]});
    return node("=", {right->children[0], gamma});
}

ASTNode* standardizeRec(ASTNode* root) {
    // rec X = E  ==>  X = gamma(ystar, lambda(X, E))
    ASTNode* def = root->children[0];
    if (def->label != "=" || def->children.size() != 2) {
        throw runtime_error("rec standardization expects '=' definition");
    }
    ASTNode* lambda = node("lambda", {cloneTree(def->children[0]), def->children[1]});
    ASTNode* gamma = node("gamma", {new ASTNode("ystar"), lambda});
    return node("=", {def->children[0], gamma});
}

ASTNode* nestedLambda(const vector<ASTNode*>& children, int start, int end) {
    ASTNode* body = children[end];
    for (int i = end - 1; i >= start; --i) {
        body = node("lambda", {children[i], body});
    }
    return body;
}

ASTNode* standardizeFcnForm(ASTNode* root) {
    // F V1 ... Vn = E  ==>  F = lambda(V1, lambda(...lambda(Vn, E)))
    if (root->children.size() < 3) {
        throw runtime_error("fcn_form standardization expects name, parameters, and body");
    }
    ASTNode* name = root->children[0];
    ASTNode* lambda = nestedLambda(root->children, 1, static_cast<int>(root->children.size()) - 1);
    return node("=", {name, lambda});
}

ASTNode* standardizeLambda(ASTNode* root) {
    if (root->children.size() < 2) {
        throw runtime_error("lambda standardization expects parameter and body");
    }
    if (root->children.size() == 2) {
        return root;
    }
    return nestedLambda(root->children, 0, static_cast<int>(root->children.size()) - 1);
}

ASTNode* standardizeAnd(ASTNode* root) {
    // X1 = E1 and X2 = E2  ==>  (X1, X2) = tau(E1, E2)
    vector<ASTNode*> names;
    vector<ASTNode*> values;
    for (ASTNode* child : root->children) {
        if (child->label != "=" || child->children.size() != 2) {
            throw runtime_error("and standardization expects '=' children");
        }
        names.push_back(child->children[0]);
        values.push_back(child->children[1]);
    }
    return node("=", {node(",", names), node("tau", values)});
}

ASTNode* standardizeAt(ASTNode* root) {
    ASTNode* inner = node("gamma", {root->children[1], root->children[0]});
    return node("gamma", {inner, root->children[2]});
}
}

ASTNode* standardize(ASTNode* root) {
    if (root == nullptr) {
        return nullptr;
    }

    ASTNode* work = cloneTree(root);
    for (ASTNode*& child : work->children) {
        ASTNode* old = child;
        child = standardize(child);
        freeTree(old);
    }

    ASTNode* result = work;
    if (work->label == "let") {
        result = standardizeLet(work);
    } else if (work->label == "where") {
        result = standardizeWhere(work);
    } else if (work->label == "within") {
        result = standardizeWithin(work);
    } else if (work->label == "rec") {
        result = standardizeRec(work);
    } else if (work->label == "fcn_form") {
        result = standardizeFcnForm(work);
    } else if (work->label == "lambda") {
        result = standardizeLambda(work);
    } else if (work->label == "and") {
        result = standardizeAnd(work);
    } else if (work->label == "@") {
        result = standardizeAt(work);
    }

    if (result != work) {
        work->children.clear();
        delete work;
    }
    return result;
}

bool isStandardized(ASTNode* node) {
    if (node == nullptr) {
        return true;
    }
    static const vector<string> rawLabels = {
        "let", "where", "within", "rec", "fcn_form", "and", "@"
    };
    for (const string& label : rawLabels) {
        if (node->label == label) {
            return false;
        }
    }
    if (node->label == "lambda" && node->children.size() != 2) {
        return false;
    }
    for (ASTNode* child : node->children) {
        if (!isStandardized(child)) {
            return false;
        }
    }
    return true;
}

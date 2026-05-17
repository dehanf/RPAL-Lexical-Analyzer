#include "CSEMachine.h"

#include "Builtins.h"

#include <cmath>
#include <stdexcept>

using namespace std;

namespace {
string unquoteString(const string& text) {
    if (text.size() >= 2 && text.front() == '\'' && text.back() == '\'') {
        return text.substr(1, text.size() - 2);
    }
    return text;
}

bool valueEquals(const RuntimeValue& left, const RuntimeValue& right) {
    if (left.type != right.type) {
        return false;
    }
    switch (left.type) {
        case RuntimeType::INT: return left.intValue == right.intValue;
        case RuntimeType::BOOL: return left.boolValue == right.boolValue;
        case RuntimeType::STRING: return left.stringValue == right.stringValue;
        case RuntimeType::NIL:
        case RuntimeType::DUMMY:
            return true;
        case RuntimeType::TUPLE:
            if (left.tupleValue.size() != right.tupleValue.size()) return false;
            for (size_t i = 0; i < left.tupleValue.size(); ++i) {
                if (!valueEquals(left.tupleValue[i], right.tupleValue[i])) return false;
            }
            return true;
        default:
            return false;
    }
}

int expectInt(const RuntimeValue& value, const string& op) {
    if (value.type != RuntimeType::INT) {
        throw runtime_error(op + " expects integer operands");
    }
    return value.intValue;
}

bool expectBool(const RuntimeValue& value, const string& op) {
    if (value.type != RuntimeType::BOOL) {
        throw runtime_error(op + " expects truth-value operands");
    }
    return value.boolValue;
}
}

CSEMachine::CSEMachine()
    : globalEnv(make_shared<Environment>()) {
    installBuiltins(*globalEnv);
}

RuntimeValue CSEMachine::evaluate(ASTNode* standardizedRoot) {
    return eval(standardizedRoot, globalEnv);
}

void CSEMachine::generateControl(ASTNode*) {}
void CSEMachine::applyGamma() {}
void CSEMachine::enterEnvironment() {}
void CSEMachine::exitEnvironment() {}

RuntimeValue CSEMachine::evalIdentifier(const string& name, shared_ptr<Environment> env) {
    return env->lookup(name);
}

RuntimeValue CSEMachine::eval(ASTNode* node, shared_ptr<Environment> env) {
    if (node == nullptr) {
        throw runtime_error("cannot evaluate null AST node");
    }

    if (node->label == "<ID>") return evalIdentifier(node->value, env);
    if (node->label == "<INT>") return makeInt(stoi(node->value));
    if (node->label == "<STR>") return makeString(unquoteString(node->value));
    if (node->label == "true") return makeBool(true);
    if (node->label == "false") return makeBool(false);
    if (node->label == "nil") return makeNil();
    if (node->label == "dummy") return makeDummy();
    if (node->label == "ystar") return makeYStar();

    if (node->label == "lambda") {
        return makeClosure(node->children[0], node->children[1], env);
    }
    if (node->label == "gamma") {
        return evaluateGamma(node, env);
    }
    if (node->label == "tau") {
        return buildTupleFromTau(node, env);
    }
    if (node->label == "aug") {
        RuntimeValue left = eval(node->children[0], env);
        RuntimeValue right = eval(node->children[1], env);
        vector<RuntimeValue> values;
        if (left.type == RuntimeType::TUPLE) values = left.tupleValue;
        else if (left.type != RuntimeType::NIL) values.push_back(left);
        values.push_back(right);
        return makeTuple(values);
    }
    if (node->label == "->") {
        return evaluateConditional(node, env);
    }
    if (node->label == "neg" || node->label == "not") {
        return evalUnaryOperator(node->label, eval(node->children[0], env));
    }
    if (node->children.size() == 2) {
        RuntimeValue left = eval(node->children[0], env);
        RuntimeValue right = eval(node->children[1], env);
        return evalOperator(node->label, left, right);
    }

    throw runtime_error("unsupported AST node during evaluation: " + node->label);
}

RuntimeValue CSEMachine::evalOperator(const string& op, const RuntimeValue& left, const RuntimeValue& right) {
    if (op == "+") return makeInt(expectInt(left, op) + expectInt(right, op));
    if (op == "-") return makeInt(expectInt(left, op) - expectInt(right, op));
    if (op == "*") return makeInt(expectInt(left, op) * expectInt(right, op));
    if (op == "/") return makeInt(expectInt(left, op) / expectInt(right, op));
    if (op == "**") return makeInt(static_cast<int>(pow(expectInt(left, op), expectInt(right, op))));
    if (op == "or") return makeBool(expectBool(left, op) || expectBool(right, op));
    if (op == "&") return makeBool(expectBool(left, op) && expectBool(right, op));
    if (op == "gr") return makeBool(expectInt(left, op) > expectInt(right, op));
    if (op == "ge") return makeBool(expectInt(left, op) >= expectInt(right, op));
    if (op == "ls") return makeBool(expectInt(left, op) < expectInt(right, op));
    if (op == "le") return makeBool(expectInt(left, op) <= expectInt(right, op));
    if (op == "eq") return makeBool(valueEquals(left, right));
    if (op == "ne") return makeBool(!valueEquals(left, right));
    throw runtime_error("unsupported binary operator: " + op);
}

RuntimeValue CSEMachine::evalUnaryOperator(const string& op, const RuntimeValue& value) {
    if (op == "neg") return makeInt(-expectInt(value, op));
    if (op == "not") return makeBool(!expectBool(value, op));
    throw runtime_error("unsupported unary operator: " + op);
}

void CSEMachine::bindPattern(shared_ptr<Environment> env, ASTNode* pattern, const RuntimeValue& value) {
    if (pattern->label == "<ID>") {
        env->bind(pattern->value, value);
        return;
    }
    if (pattern->label == ",") {
        if (value.type != RuntimeType::TUPLE ||
            value.tupleValue.size() != pattern->children.size()) {
            throw runtime_error("tuple binding arity mismatch");
        }
        for (size_t i = 0; i < pattern->children.size(); ++i) {
            bindPattern(env, pattern->children[i], value.tupleValue[i]);
        }
        return;
    }
    if (pattern->label == "()") {
        return;
    }
    throw runtime_error("unsupported binding pattern: " + pattern->label);
}

RuntimeValue CSEMachine::applyClosure(const RuntimeValue& closure, const RuntimeValue& argument) {
    // Closures keep the environment where the lambda was created, giving RPAL
    // lexical scoping. Application extends that saved environment, then binds
    // the formal parameter pattern to the evaluated argument.
    shared_ptr<Environment> env = closure.environment->extend();
    bindPattern(env, closure.parameter, argument);
    return eval(closure.body, env);
}

RuntimeValue CSEMachine::evaluateGamma(ASTNode* node, shared_ptr<Environment> env) {
    RuntimeValue function = eval(node->children[0], env);
    RuntimeValue argument = eval(node->children[1], env);

    if (function.type == RuntimeType::CLOSURE) {
        return applyClosure(function, argument);
    }
    if (function.type == RuntimeType::BUILTIN) {
        return applyBuiltin(function.builtinName, function.builtinArgs, argument);
    }
    if (function.type == RuntimeType::YSTAR) {
        // The standardizer represents rec with ystar. Applying ystar ties the
        // closure's name back to the resulting function inside a recursive
        // environment, so calls from the function body can find themselves.
        if (argument.type != RuntimeType::CLOSURE || argument.parameter->label != "<ID>") {
            throw runtime_error("ystar expects a closure with identifier parameter");
        }
        shared_ptr<Environment> recEnv = argument.environment->extend();
        RuntimeValue result = eval(argument.body, recEnv);
        if (result.type == RuntimeType::CLOSURE) {
            result.environment = recEnv;
        }
        recEnv->bind(argument.parameter->value, result);
        return result;
    }
    if ((function.type == RuntimeType::TUPLE || function.type == RuntimeType::NIL) &&
        argument.type == RuntimeType::INT) {
        // RPAL tuple selection is function application: (a, b, c) 2 ==> b.
        const vector<RuntimeValue> empty;
        const vector<RuntimeValue>& values =
            function.type == RuntimeType::TUPLE ? function.tupleValue : empty;
        int index = argument.intValue;
        if (index < 1 || index > static_cast<int>(values.size())) {
            throw runtime_error("tuple index out of range");
        }
        return values[index - 1];
    }
    throw runtime_error("gamma cannot apply non-function value");
}

RuntimeValue CSEMachine::buildTupleFromTau(ASTNode* node, shared_ptr<Environment> env) {
    vector<RuntimeValue> values;
    for (ASTNode* child : node->children) {
        values.push_back(eval(child, env));
    }
    return makeTuple(values);
}

RuntimeValue CSEMachine::evaluateConditional(ASTNode* node, shared_ptr<Environment> env) {
    RuntimeValue condition = eval(node->children[0], env);
    if (condition.type != RuntimeType::BOOL) {
        throw runtime_error("conditional guard must be a truth value");
    }
    return eval(condition.boolValue ? node->children[1] : node->children[2], env);
}

#include "RuntimeValue.h"

#include <sstream>
#include <stdexcept>

using namespace std;

RuntimeValue makeInt(int value) {
    RuntimeValue v;
    v.type = RuntimeType::INT;
    v.intValue = value;
    return v;
}

RuntimeValue makeBool(bool value) {
    RuntimeValue v;
    v.type = RuntimeType::BOOL;
    v.boolValue = value;
    return v;
}

RuntimeValue makeString(const string& value) {
    RuntimeValue v;
    v.type = RuntimeType::STRING;
    v.stringValue = value;
    return v;
}

RuntimeValue makeTuple(const vector<RuntimeValue>& values) {
    RuntimeValue v;
    v.type = RuntimeType::TUPLE;
    v.tupleValue = values;
    return v;
}

RuntimeValue makeNil() {
    RuntimeValue v;
    v.type = RuntimeType::NIL;
    return v;
}

RuntimeValue makeDummy() {
    RuntimeValue v;
    v.type = RuntimeType::DUMMY;
    return v;
}

RuntimeValue makeClosure(ASTNode* parameter, ASTNode* body, shared_ptr<Environment> environment) {
    RuntimeValue v;
    v.type = RuntimeType::CLOSURE;
    v.parameter = parameter;
    v.body = body;
    v.environment = environment;
    return v;
}

RuntimeValue makeBuiltin(const string& name) {
    return makeBuiltin(name, {});
}

RuntimeValue makeBuiltin(const string& name, const vector<RuntimeValue>& args) {
    RuntimeValue v;
    v.type = RuntimeType::BUILTIN;
    v.builtinName = name;
    v.builtinArgs = args;
    return v;
}

RuntimeValue makeYStar() {
    RuntimeValue v;
    v.type = RuntimeType::YSTAR;
    return v;
}

string formatStringOutput(const string& value) {
    string out;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 1 < value.size()) {
            char next = value[++i];
            if (next == 'n') out += '\n';
            else if (next == 't') out += '\t';
            else out += next;
        } else {
            out += value[i];
        }
    }
    return out;
}

string formatTupleOutput(const RuntimeValue& tuple) {
    if (tuple.type != RuntimeType::TUPLE) {
        throw runtime_error("formatTupleOutput expects tuple");
    }
    stringstream ss;
    ss << "(";
    for (size_t i = 0; i < tuple.tupleValue.size(); ++i) {
        if (i > 0) {
            ss << ", ";
        }
        ss << formatRuntimeValue(tuple.tupleValue[i]);
    }
    ss << ")";
    return ss.str();
}

string formatRuntimeValue(const RuntimeValue& value) {
    switch (value.type) {
        case RuntimeType::INT:
            return to_string(value.intValue);
        case RuntimeType::BOOL:
            return value.boolValue ? "true" : "false";
        case RuntimeType::STRING:
            return formatStringOutput(value.stringValue);
        case RuntimeType::TUPLE:
            return formatTupleOutput(value);
        case RuntimeType::NIL:
            return "nil";
        case RuntimeType::DUMMY:
            return "dummy";
        case RuntimeType::CLOSURE:
            return "[function]";
        case RuntimeType::BUILTIN:
            return "[builtin:" + value.builtinName + "]";
        case RuntimeType::YSTAR:
            return "[ystar]";
    }
    return "";
}

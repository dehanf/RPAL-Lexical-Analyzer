#include "Builtins.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace {
int arity(const string& name) {
    if (name == "Conc") {
        return 2;
    }
    return 1;
}

bool isNilLike(const RuntimeValue& value) {
    return value.type == RuntimeType::NIL ||
           (value.type == RuntimeType::TUPLE && value.tupleValue.empty());
}

RuntimeValue finishBuiltin(const string& name, const vector<RuntimeValue>& args) {
    const RuntimeValue& a = args[0];

    if (name == "Print") {
        // Print is the only built-in here with a side effect; normal evaluation
        // does not print returned dummy values.
        cout << formatRuntimeValue(a) << endl;
        return makeDummy();
    }
    if (name == "Conc") {
        if (args[0].type != RuntimeType::STRING || args[1].type != RuntimeType::STRING) {
            throw runtime_error("Conc expects two strings");
        }
        return makeString(args[0].stringValue + args[1].stringValue);
    }
    if (name == "Stem") {
        if (a.type != RuntimeType::STRING) throw runtime_error("Stem expects string");
        return makeString(a.stringValue.empty() ? "" : a.stringValue.substr(0, 1));
    }
    if (name == "Stern") {
        if (a.type != RuntimeType::STRING) throw runtime_error("Stern expects string");
        return makeString(a.stringValue.size() <= 1 ? "" : a.stringValue.substr(1));
    }
    if (name == "Order") {
        if (a.type == RuntimeType::TUPLE) return makeInt(static_cast<int>(a.tupleValue.size()));
        if (a.type == RuntimeType::NIL) return makeInt(0);
        throw runtime_error("Order expects tuple");
    }
    if (name == "Null") {
        return makeBool(isNilLike(a));
    }
    if (name == "ItoS") {
        if (a.type != RuntimeType::INT) throw runtime_error("ItoS expects integer");
        return makeString(to_string(a.intValue));
    }
    if (name == "Isinteger") return makeBool(a.type == RuntimeType::INT);
    if (name == "Istruthvalue") return makeBool(a.type == RuntimeType::BOOL);
    if (name == "Isstring") return makeBool(a.type == RuntimeType::STRING);
    if (name == "Istuple") return makeBool(a.type == RuntimeType::TUPLE || a.type == RuntimeType::NIL);
    if (name == "Isfunction") return makeBool(a.type == RuntimeType::CLOSURE || a.type == RuntimeType::BUILTIN);
    if (name == "Isdummy") return makeBool(a.type == RuntimeType::DUMMY);

    throw runtime_error("unknown builtin: " + name);
}
}

void installBuiltins(Environment& env) {
    const vector<string> names = {
        "Print", "Conc", "Stem", "Stern", "Order", "Null", "ItoS",
        "Isinteger", "Istruthvalue", "Isstring", "Istuple", "Isfunction",
        "Isdummy"
    };
    for (const string& name : names) {
        env.bind(name, makeBuiltin(name));
    }
}

RuntimeValue applyBuiltin(const string& name,
                          const vector<RuntimeValue>& existingArgs,
                          const RuntimeValue& arg) {
    vector<RuntimeValue> args = existingArgs;
    args.push_back(arg);
    if (static_cast<int>(args.size()) < arity(name)) {
        return makeBuiltin(name, args);
    }
    return finishBuiltin(name, args);
}

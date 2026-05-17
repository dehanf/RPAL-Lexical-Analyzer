#include "Environment.h"

#include "RuntimeValue.h"

#include <stdexcept>

using namespace std;

Environment::Environment(shared_ptr<Environment> parentEnv)
    : parent(parentEnv) {}

void Environment::bind(const string& name, const RuntimeValue& value) {
    bindings[name] = value;
}

RuntimeValue Environment::lookup(const string& name) const {
    auto it = bindings.find(name);
    if (it != bindings.end()) {
        return it->second;
    }
    if (parent) {
        return parent->lookup(name);
    }
    throw runtime_error("unbound identifier: " + name);
}

shared_ptr<Environment> Environment::extend() {
    return make_shared<Environment>(shared_from_this());
}

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <memory>
#include <string>

#include "RuntimeValue.h"

class Environment : public std::enable_shared_from_this<Environment> {
public:
    explicit Environment(std::shared_ptr<Environment> parent = nullptr);

    void bind(const std::string& name, const RuntimeValue& value);
    RuntimeValue lookup(const std::string& name) const;
    std::shared_ptr<Environment> extend();

private:
    std::map<std::string, RuntimeValue> bindings;
    std::shared_ptr<Environment> parent;
};

#endif

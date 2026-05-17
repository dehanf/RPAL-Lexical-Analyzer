#ifndef RUNTIMEVALUE_H
#define RUNTIMEVALUE_H

#include <memory>
#include <string>
#include <vector>

struct ASTNode;
class Environment;

enum class RuntimeType {
    INT,
    BOOL,
    STRING,
    TUPLE,
    NIL,
    DUMMY,
    CLOSURE,
    BUILTIN,
    YSTAR
};

struct RuntimeValue {
    RuntimeType type = RuntimeType::DUMMY;
    int intValue = 0;
    bool boolValue = false;
    std::string stringValue;
    std::vector<RuntimeValue> tupleValue;
    ASTNode* parameter = nullptr;
    ASTNode* body = nullptr;
    std::shared_ptr<Environment> environment;
    std::string builtinName;
    std::vector<RuntimeValue> builtinArgs;
};

RuntimeValue makeInt(int value);
RuntimeValue makeBool(bool value);
RuntimeValue makeString(const std::string& value);
RuntimeValue makeTuple(const std::vector<RuntimeValue>& values);
RuntimeValue makeNil();
RuntimeValue makeDummy();
RuntimeValue makeClosure(ASTNode* parameter, ASTNode* body, std::shared_ptr<Environment> environment);
RuntimeValue makeBuiltin(const std::string& name);
RuntimeValue makeBuiltin(const std::string& name, const std::vector<RuntimeValue>& args);
RuntimeValue makeYStar();

std::string formatRuntimeValue(const RuntimeValue& value);
std::string formatStringOutput(const std::string& value);
std::string formatTupleOutput(const RuntimeValue& tuple);

#endif

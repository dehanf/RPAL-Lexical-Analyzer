#ifndef CSEMACHINE_H
#define CSEMACHINE_H

#include "ASTNode.h"
#include "Environment.h"
#include "RuntimeValue.h"

#include <memory>

class CSEMachine {
public:
    CSEMachine();
    RuntimeValue evaluate(ASTNode* standardizedRoot);

    void generateControl(ASTNode* node);
    void applyGamma();
    void enterEnvironment();
    void exitEnvironment();

private:
    std::shared_ptr<Environment> globalEnv;

    RuntimeValue eval(ASTNode* node, std::shared_ptr<Environment> env);
    RuntimeValue evalIdentifier(const std::string& name, std::shared_ptr<Environment> env);
    RuntimeValue evalOperator(const std::string& op, const RuntimeValue& left, const RuntimeValue& right);
    RuntimeValue evalUnaryOperator(const std::string& op, const RuntimeValue& value);
    RuntimeValue applyClosure(const RuntimeValue& closure, const RuntimeValue& argument);
    RuntimeValue evaluateGamma(ASTNode* node, std::shared_ptr<Environment> env);
    RuntimeValue evaluateConditional(ASTNode* node, std::shared_ptr<Environment> env);
    RuntimeValue buildTupleFromTau(ASTNode* node, std::shared_ptr<Environment> env);
    void bindPattern(std::shared_ptr<Environment> env, ASTNode* pattern, const RuntimeValue& value);
};

#endif

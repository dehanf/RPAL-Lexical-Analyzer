#ifndef BUILTINS_H
#define BUILTINS_H

#include "Environment.h"
#include "RuntimeValue.h"

void installBuiltins(Environment& env);
RuntimeValue applyBuiltin(const std::string& name,
                          const std::vector<RuntimeValue>& existingArgs,
                          const RuntimeValue& arg);

#endif

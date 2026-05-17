#ifndef LEXICAL_H
#define LEXICAL_H

#include <string>
#include <vector>
#include "Token.h"

std::vector<Token> tokenize(const std::string& fileName);
std::string tokenTypeToString(TokenType type);

#endif

#pragma once 
#include <unordered_set>

namespace Interpreter 
{

class Token;

using Lexicon = std::unordered_set<const Token*>;

}
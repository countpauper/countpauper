#pragma once 

#include "Terms.h"
#include "SourceSpan.h"
#include "Rule.h"
#include <vector>
#include <string>


namespace Interpreter 
{
    using Terms = std::vector<Term>;
    struct Rule
    {
        std::string symbol;
        Terms terms;
        SourceSpan location = {0,0}; // set if loaded from source else 0,0
        operator std::string() const;
        hash_t SymbolHash() const;
        const Term* FindLeft(std::function<bool(const Term&)> predicate) const;
    };

}
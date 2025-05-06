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
        Symbol symbol;
        Terms terms;
        SourceSpan location = {0,0}; // set if loaded from source else 0,0

        operator std::string() const;
        const Term* FindLeft(std::function<bool(const Term&)> predicate) const;
    };

}
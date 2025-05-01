#pragma once 

#include "Terms.h"
#include "Lexicon.h"
#include "SourceSpan.h"
#include <string>
#include <list>
#include <variant>
#include <map>
#include <ranges>

namespace Interpreter 
{
    using Terms = std::vector<Term>;
    struct Rule
    {
        std::string name;
        Terms terms;
        SourceSpan location = {0,0}; // set if loaded from source else 0,0
        operator std::string() const;
        hash_t Hash() const;
    };

    class Syntax : public std::list<Rule>
    {
    public:
        explicit Syntax(std::initializer_list<Rule> rules={}, hash_t start=0);
        using LookupTable = std::multimap<hash_t, const Terms*>;
        std::ranges::subrange<LookupTable::const_iterator> Lookup(hash_t symbol) const;
        hash_t Start() const;
    private:
        void CreateLookup();
        hash_t start;
        LookupTable lookup;
    };
}
#pragma once 

#include "Tokens.h"
#include "Lexicon.h"
#include "SourceSpan.h"
#include <string>
#include <list>
#include <variant>
#include <map>

namespace Interpreter 
{
    struct Symbol 
    {
        std::string name;
        operator std::string() const;
    };

    class Rule; 

    using Term = std::variant<Symbol, Literal, Regex>; 
    std::string to_string(const Term& term);

    using Terms = std::vector<Term>;
    struct Rule
    {
        std::string name;
        Terms terms;
        SourceSpan location = {0,0}; // set if loaded from source else 0,0
        operator std::string() const;
    };

    class Syntax : public std::list<Rule>
    {
    public:
        explicit Syntax(std::initializer_list<Rule> rules={});
        using LookupTable = std::multimap<std::string_view, const Terms*>;
        
        class Range
        {
        public:
            Range(std::pair<LookupTable::const_iterator, LookupTable::const_iterator> it);
            bool empty() const;
            std::size_t size() const;
            LookupTable::const_iterator begin() const;
            LookupTable::const_iterator end() const;
        private:
            LookupTable::const_iterator b, e;    
        };
        Range Lookup(const std::string_view symbol) const;
    private:
        void CreateLookup();
        LookupTable lookup;
    };
}
#pragma once 

#include "Tokens.h"
#include "Lexicon.h"
#include <string>
#include <list>
#include <variant>
#include <map>

namespace Interpreter 
{
    using Symbol = std::string;
    class Rule; 

    using Term = std::variant<Symbol, Literal, Regex>; 
    using Terms = std::vector<Term>;
    struct Rule
    {
        Symbol name;
        Terms terms;
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
        Range Lookup(const Symbol& symbol) const;
    private:
        void CreateLookup();
        LookupTable lookup;
    };
}
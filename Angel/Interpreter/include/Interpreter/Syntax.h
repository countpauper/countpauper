#pragma once 

#include "Tokens.h"
#include "Lexicon.h"
#include <string>
#include <list>
#include <variant>

namespace Interpreter 
{
    using Symbol = std::string;
    class Rule; 

    using Term = std::variant<Symbol, Literal, Regex>;

    struct Rule
    {
        Symbol name;
        std::vector<Term> terms;
    };


    class Syntax : public std::list<Rule>
    {
    public:
        explicit Syntax(std::initializer_list<Rule> rules={});
        const Lexicon& GetLexicon() const;
    private:
        Lexicon lexicon;
    };
}
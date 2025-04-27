#include "Interpreter/Lexicon.h"
#include "Interpreter/Syntax.h"

namespace Interpreter 
{

Lexicon::Lexicon(std::initializer_list<const Token*> tokens) :
    std::unordered_set<const Token*>(tokens)
{
}

void ExtendLexicon(Lexicon& lexicon, const Rule& rule)
{
    for(const auto& term : rule.terms)
    {
        if (auto literal = get_if<Literal>(&term))
        {
            lexicon.insert(literal);
        }
        else if (auto regex = get_if<Regex>(&term))
        {
            lexicon.insert(regex);
        }
    }
}

Lexicon::Lexicon(const Syntax& syntax)
{
    std::for_each(syntax.begin(), syntax.end(),[this](const Rule& rule)
    {
        ExtendLexicon(*this, rule);
    });

}

}
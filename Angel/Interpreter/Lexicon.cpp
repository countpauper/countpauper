#include "Interpreter/Lexicon.h"
#include "Interpreter/Syntax.h"
#include <assert.h>

namespace Interpreter 
{

Lexicon::Lexicon(std::initializer_list<const Token*> tokens)
{
    for(auto token: tokens)
    {
        auto ins = emplace(token->Hash(), token);
        assert(ins.second); // hash must be unique
    }
}

void ExtendLexicon(Lexicon& lexicon, const Rule& rule)
{
    for(const auto& term : rule.terms)
    {
        // TODO also put symbols in 
        if (auto literal = get_if<Literal>(&term))
        {
            [[maybe_unused]] auto ins = lexicon.emplace(literal->Hash(), literal);
            assert(ins.second || *literal == *ins.first->second);
        }
        else if (auto regex = get_if<Regex>(&term))
        {
            [[maybe_unused]] auto ins = lexicon.emplace(regex->Hash(), regex);
            assert(ins.second || *regex == *ins.first->second);
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
#include "Parser/Lexicon.h"
#include "Parser/Syntax.h"
#include "Parser/Utils.h"
#include <assert.h>

namespace Parser 
{


void ExtendLexicon(Lexicon& lexicon, const Term& term)
{
    return std::visit(overloaded_visit{
        [](const Symbol&) {},
        [&lexicon, &term]<is_token _Token>(const _Token& token ) 
        { 
            [[maybe_unused]] auto ins = lexicon.emplace(std::hash<Term>()(term), &term);
            assert(ins.second || term == *ins.first->second);
        }
    }, term);
}

void ExtendLexicon(Lexicon& lexicon, const Rule& rule)
{
    for(const auto& term : rule.terms)
    {
        ExtendLexicon(lexicon, term);
    }
}

Lexicon::Lexicon(std::initializer_list<const Term*> terms)
{
    for(const auto term: terms)
    {
        ExtendLexicon(*this, *term);
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
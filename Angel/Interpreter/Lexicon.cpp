#include "Interpreter/Lexicon.h"
#include "Interpreter/Syntax.h"
#include <assert.h>

namespace Interpreter 
{

Lexicon::Lexicon(std::initializer_list<const Term*> terms)
{
    for(const auto& term: terms)
    {
        auto ins = emplace(std::hash<Term>()(*term), term);
        assert(ins.second); // hash must be unique
    }
}

void ExtendLexicon(Lexicon& lexicon, const Rule& rule)
{
    for(const auto& term : rule.terms)
    {
        [[maybe_unused]] auto ins = lexicon.emplace(std::hash<Term>()(term), &term);
        assert(ins.second || term == *ins.first->second);
    }
}

Lexicon::Lexicon(const Syntax& syntax)
{
    AddRoot(syntax);
    std::for_each(syntax.begin(), syntax.end(),[this](const Rule& rule)
    {
        ExtendLexicon(*this, rule);
    });

}

void Lexicon::AddRoot(const Syntax& syntax)
{
    const auto& syntaxRoot = syntax.Root();
    if (syntaxRoot)
    {
        root = syntaxRoot;
        emplace(syntaxRoot.Hash(), &root.value());
    }
}

}
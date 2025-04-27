#include "Interpreter/Syntax.h"

namespace Interpreter 
{


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
Lexicon BuildLexicon(const Syntax& syntax)
{
    Lexicon result;
    std::for_each(syntax.begin(), syntax.end(),[&result](const Rule& rule)
    {
        ExtendLexicon(result, rule);
    });
    return result;
}

Syntax::Syntax(std::initializer_list<Rule> rules) :
    std::list<Rule>(rules),
    lexicon(BuildLexicon(*this))    // cache it
{
}

const Lexicon& Syntax::GetLexicon() const
{
    return lexicon;
}

}
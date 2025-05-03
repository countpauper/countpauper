#include "Interpreter/Syntax.h"
#include "Interpreter/Utils.h"
#include <type_traits>
#include <variant>
#include <cassert>

namespace Interpreter 
{

Syntax::Syntax(std::initializer_list<Rule> input, const std::string_view start) 
{
    if (!start.empty()) {
        root = std::hash<Term>()(Symbol(std::string(start)));
    }
    else if (input.size())
    {
        this->root = input.begin()->SymbolHash();
    }
    for(const auto& rule:input)
    {
        lookup.emplace(rule.SymbolHash(), std::move(rule));
    }
}


bool Syntax::empty() const
{
    return lookup.empty();
}

Syntax::iterator Syntax::begin() const
{
    return iterator(lookup.begin());
}

Syntax::iterator Syntax::end() const
{
    return iterator(lookup.end());
}

std::ranges::subrange<Syntax::LookupTable::const_iterator> Syntax::Lookup(hash_t symbol) const
{
    auto [first, last] = lookup.equal_range(symbol); 
    return std::ranges::subrange(first, last);
}

std::ranges::subrange<Syntax::LookupTable::const_iterator> Syntax::operator[](hash_t symbol) const
{
    return Lookup(symbol);
}

hash_t Syntax::Root() const
{
    return root;
}

const Term* FindLeftSymbolOrNotEpsilon(const Rule& rule)
{
    return rule.FindLeft([](const Term& term)
    {
        return std::visit(overloaded_visit{
            [](const Symbol&) { return true; },
            [](const Token& token) { return !IsEpsilon(token); }
        }, term);
    });
}

bool Syntax::CheckLeftRecursive(SyntaxPath& path) const 
{
    assert(!path.empty());
    for(const auto& rule: Lookup(path.back()))
    {
        if (rule.second.terms.empty())
            continue;
        auto left = FindLeftSymbolOrNotEpsilon(rule.second);
        if (!left)
            continue;

        if (std::visit(overloaded_visit{
            [this, &path](const Symbol& term) 
            {
                hash_t termHash = std::hash<Term>()(term);
                if (std::find(path.begin(), path.end(), termHash)!=path.end())
                    return true;
                path.push_back(termHash);
                auto result = CheckLeftRecursive(path);
                assert(!path.empty() && path.back() == termHash);
                path.pop_back();
                return result; 
            },
            [](const Token&) { return false; }}
            , *left)) 
            return true;
    }
    return false;
}

bool Syntax::IsLeftRecursive() const
{
    SyntaxPath path;
    path.push_back(Root());
    return CheckLeftRecursive(path);

}

}
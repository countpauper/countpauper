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
        root = Symbol(start);
    }
    else if (input.size())
    {
        this->root = input.begin()->symbol;
    }
    for(const auto& rule:input)
    {
        lookup.emplace(rule.symbol, std::move(rule));
    }
}

Syntax::Syntax(Syntax&& other)
{
    std::swap(root, other.root);
    std::swap(lookup, other.lookup);
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

std::ranges::subrange<Syntax::LookupTable::const_iterator> Syntax::Lookup(Symbol symbol) const
{
    auto [first, last] = lookup.equal_range(symbol); 
    return std::ranges::subrange(first, last);
}

std::ranges::subrange<Syntax::LookupTable::const_iterator> Syntax::operator[](Symbol symbol) const
{
    return Lookup(symbol);
}

Symbol Syntax::Root() const
{
    return root;
}

const Term* FindLeftSymbolOrNotEpsilon(const Rule& rule)
{
    return rule.FindLeft([](const Term& term)
    {
        return std::visit(overloaded_visit{
            [](const Symbol&) { return true; },
            [](const Epsilon&) { return false; },
            []<is_token _Token>(const _Token& token ) { return true; }
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
            [this, &path](const Symbol& symbol) 
            {
                if (std::find(path.begin(), path.end(), symbol)!=path.end())
                    return true;
                path.push_back(symbol);
                auto result = CheckLeftRecursive(path);
                assert(!path.empty() && path.back() == symbol);
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
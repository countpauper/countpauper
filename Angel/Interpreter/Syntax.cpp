#include "Interpreter/Syntax.h"
#include <numeric>
#include <type_traits>
#include <variant>

namespace Interpreter 
{

Symbol::operator std::string() const
{
    return std::format("<{}>", name);
}

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

Rule::operator std::string() const 
{
    std::string termstr;
    std::for_each(terms.begin(), terms.end(), [&termstr](const Term& term)
    {
        termstr += to_string(term) + " ";
    });
    return std::format("{}::={}", std::string(symbol), termstr.substr(0, termstr.size()-1));
}

hash_t Rule::SymbolHash() const 
{ 
    return std::hash<Term>()(Symbol(symbol)); 
}

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

/*
Syntax::Range::Range(std::pair<LookupTable::const_iterator, LookupTable::const_iterator> it) :
    b(it.first),
    e(it.second)
{

}
bool Syntax::Range::empty() const
{
    return size() == 0;
}
std::size_t Syntax::Range::size() const
{
    return std::distance(begin(), end());
}

Syntax::LookupTable::const_iterator Syntax::Range::begin() const
{
    return b;
}

Syntax::LookupTable::const_iterator Syntax::Range::end() const
{
    return e;
}
*/

}
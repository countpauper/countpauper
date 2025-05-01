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
    return std::format("{}::={}", std::string(name), termstr.substr(0, termstr.size()-1));
}

hash_t Rule::Hash() const 
{ 
    return std::hash<Term>()(Symbol(name)); 
}

Syntax::Syntax(std::initializer_list<Rule> rules, const std::string_view start) :
    std::list<Rule>(rules)
{
    if (!start.empty()) {
        root = std::hash<Term>()(Symbol(std::string(start)));
    }
    else if (!empty())
    {
        this->root = front().Hash();
    }
    CreateLookup();
}

void Syntax::CreateLookup() 
{
    lookup.clear();
    for(const Rule& rule: *this)
    {   // NB: this is a somewhat risky optimization. The lookup creates views on the strings
        // As long as the rules list is made, it has allocated these strings and they should not 
        // move and can be referenced. If an element is removed, the string view will contain 
        // a dangling pointer. 
        // Since the list is a public base class (to simplify inherting container interface)
        // this is not guaranteed as the user of the syntax may erase elements after constrution 
        // which would make the lookup table invalid.
        lookup.emplace(rule.Hash(), &rule);
    }
}

std::ranges::subrange<Syntax::LookupTable::const_iterator> Syntax::Lookup(hash_t symbol) const
{
    auto [first, last] = lookup.equal_range(symbol); 
    return std::ranges::subrange(first, last);

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
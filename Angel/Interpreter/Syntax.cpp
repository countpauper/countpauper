#include "Interpreter/Syntax.h"

namespace Interpreter 
{

Syntax::Syntax(std::initializer_list<Rule> rules) :
    std::list<Rule>(rules)
{
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
        lookup.emplace(rule.name, &rule.terms);
    }
}

Syntax::Range Syntax::Lookup(const Symbol& symbol) const
{
    return Range(lookup.equal_range(symbol));
}

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
}
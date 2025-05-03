#include "Interpreter/Rule.h"
#include <numeric>

namespace Interpreter 
{

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


const Term* Rule::FindLeft(std::function<bool(const Term&)> predicate) const
{  
    auto it = std::find_if(terms.begin(), terms.end(), predicate);
    if (it!=terms.end())
        return &(*it);
    return nullptr;
}

}
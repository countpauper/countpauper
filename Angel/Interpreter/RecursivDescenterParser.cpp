

#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Error.h"
#include "Interpreter/Utils.h"
#include "Interpreter/Logging.h"
#include <sstream>
#include <ranges>
#include <assert.h>

namespace Interpreter 
{

RecursiveDescentParser::RecursiveDescentParser(const Syntax& syntax) :
    Parser(syntax)
{
    if (syntax.IsLeftRecursive())
        throw std::runtime_error("Left recursion in syntax may cause infite recursion during recursive descent.");
}

static int tab;
std::string Tab() 
{
    return std::string(tab, ' ');
}

void RecursiveDescentParser::Parse(TokenStream& is, SymbolStream& os)
{
    auto root = syntax.Root();
    auto input = is.Dump();
    auto it = input.begin();
    assert(input.back().token == 0); // skip end
    tab = 0;
    auto output = Recurse(root, it, input.end()-1);
    if (output.empty())
        throw Error("Unexpected token", it->reference);
    for(const auto& symbol : output)
    {
        os << symbol;
    }
}

std::vector<OutputSymbol> RecursiveDescentParser::Recurse(Symbol symbol,
    RecursiveDescentParser::InputIterator& from, RecursiveDescentParser::InputIterator to)
{
    auto rules = syntax.Lookup(symbol);
    for(const auto& rule: rules)
    {
        Logging::Log<Logging::INFO>("{}{}[", Tab(), std::string(rule.second.symbol));
        tab++;
        std::vector<OutputSymbol> result = Recurse(rule.first, rule.second.terms, from, to);
        tab--;
        if (!result.empty()) 
        {
            Logging::Log<Logging::INFO>("{}] pass", Tab());
            return result;
        }
        else 
        {
            Logging::Log<Logging::INFO>("{}] fail", Tab());
        }
    }
    return std::vector<OutputSymbol>();
}


std::vector<OutputSymbol> RecursiveDescentParser::Recurse(Symbol symbol, const Terms& terms, 
    RecursiveDescentParser::InputIterator& from, RecursiveDescentParser::InputIterator to)
{
    std::vector<OutputSymbol> result;
    result.emplace_back(symbol, from->reference);
    auto it = from;
    for(const Term& term: terms)
    {
        auto match =  std::visit( overloaded_visit
        {
            [&it, &to, this, &result]( const Symbol& subSymbol )
            { 
                auto subResult = Recurse(subSymbol, it, to); 
                if (subResult.empty())
                    return false;
                for(const auto& i : subResult)
                    result.emplace_back(i);
                return true;
            },
            [&it, &to, this] <is_token _Token>(const _Token& token )
            { 
                if (IsEpsilon(token))
                    return true;
                if (it==to)
                    return false;
                if (std::hash<Term>()(token) != it->token) 
                    return false;
                ++it;
                return true;
            },
            []( auto&  ) 
            { 
                return false;
            }
        }, term); 

        if (!match) {
            return std::vector<OutputSymbol>();
        }
    }
    result[0].location.length = it->reference.from - from->reference.from;
    from = it;
    return result;
}

}
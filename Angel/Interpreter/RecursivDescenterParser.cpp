

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

static Logging::Tabber tab;


void RecursiveDescentParser::ParseTokens(TokenStream& is, SymbolStream& os)
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

std::vector<ParsedSymbol> RecursiveDescentParser::Recurse(Symbol symbol,
    RecursiveDescentParser::InputIterator& from, RecursiveDescentParser::InputIterator to)
{
    auto rules = syntax.Lookup(symbol);
    for(const auto& rule: rules)
    {
        Logging::Log<Logging::DEBUG>("{:4}{}{}[", from->reference.from, std::string(tab), std::string(rule.second.symbol));
        ++tab;
        std::vector<ParsedSymbol> result = Recurse(rule.first, rule.second.terms, from, to);
        --tab;
        if (!result.empty()) 
        {
            Logging::Log<Logging::DEBUG>("{:4}{}] pass", from->reference.from, std::string(tab));
            return result;
        }
        else 
        {
            Logging::Log<Logging::DEBUG>("{:4}{}] fail", from->reference.from, std::string(tab));
        }
    }
    return std::vector<ParsedSymbol>();
}


std::vector<ParsedSymbol> RecursiveDescentParser::Recurse(Symbol symbol, const Terms& terms, 
    RecursiveDescentParser::InputIterator& from, RecursiveDescentParser::InputIterator to)
{
    std::vector<ParsedSymbol> result;
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
            [&result, &it] (const Epsilon& epsilon)
            {
                if (epsilon.GetSymbol())
                    result.emplace_back(ParsedSymbol{epsilon.GetSymbol(), it->reference.sub(0,0)});
                return true;
            },
            [&it, &to, this] <is_token _Token>(const _Token& token )
            { 
                if (it==to)
                    return false;
                if (std::hash<Term>()(token) != it->token) 
                    return false;
                Logging::Log<Logging::DEBUG>("{:4}{}`{}`", it->reference.from, std::string(tab), it->reference.extract());
                ++it;
                return true;
            },
            []( auto&  ) 
            { 
                return false;
            }
        }, term); 

        if (!match) {
            if (!result.empty())
            //TODO: figure out how opt-whitespace triggers this. because without it the Dump() of the stream is no longer neccessary
            //    throw Error(std::format("Grammar is not deterministic in {}", std::string(symbol)), from->reference);
                return std::vector<ParsedSymbol>();
            return result;
        }
    }
    result.insert(result.begin(), ParsedSymbol{symbol, from->reference.sub(0, it->reference.from - from->reference.from)});
    from = it;
    return result;
}

}
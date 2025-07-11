

#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Error.h"
#include "Interpreter/Utils.h"
#include "Interpreter/Logging.h"
#include "Interpreter/Source.h"
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


void RecursiveDescentParser::ParseTokens(TokenStream& is, SymbolStream& os, const Symbol& start)
{
    tab = 0;
    auto output = Recurse(start, is);
    if (output.empty())
    {
        const auto& pos = is.peek();

        throw Error(std::format("Unexpected token {}", pos.token?
            to_string(*lexicon.at(pos.token)):"<eof>"), pos.reference);
    }
    for(const auto& symbol : output)
    {
        os << symbol;
    }
}

std::vector<ParsedSymbol> RecursiveDescentParser::Recurse(const Symbol& symbol, TokenStream& is)
{
    auto rules = syntax.Lookup(symbol);
    for(const auto& rule: rules)
    {
        const auto pos = is.peek();
        Logging::Log<Logging::DEBUG>("{:4}{}{}[", pos.reference.offset(), std::string(tab), std::string(rule.second.symbol));
        ++tab;
        std::vector<ParsedSymbol> result = Recurse(rule.first, rule.second.terms, is);
        --tab;
        if (!result.empty()) 
        {
            Logging::Log<Logging::DEBUG>("{:4}{}] pass {} {} {} terms", pos.reference.offset(), std::string(tab),
                 std::string(result.front().symbol), std::string(result.front().location), result.size()-1);
            return result;
        }
        else 
        {
            Logging::Log<Logging::DEBUG>("{:4}{}] fail {}", pos.reference.offset(), std::string(tab),
                std::string(symbol));
        }
    }
    return std::vector<ParsedSymbol>();
}


std::vector<ParsedSymbol> RecursiveDescentParser::Recurse(const Symbol& symbol, const Terms& terms, TokenStream& is)
{
    std::vector<ParsedSymbol> result;
    InputToken start = is.peek();   // TODO: maybe optimize without symbol copies
    for(const Term& term: terms)
    {
        auto match =  std::visit( overloaded_visit
        {
            [&is, this, &result]( const Symbol& subSymbol )
            { 
                auto subResult = Recurse(subSymbol, is); 
                if (subResult.empty())
                    return false;
                for(const auto& i : subResult)
                    result.emplace_back(i);
                return true;
            },
            [&result, &is] (const Epsilon& epsilon)
            {
                if (epsilon.GetSymbol())
                {
                    const auto& pos = is.peek();
                    result.emplace_back(ParsedSymbol{epsilon.GetSymbol(), pos.reference.sub(0,0)});
                }
                auto pos = is.peek();
                Logging::Log<Logging::DEBUG>("{:4}{}{}", pos.reference.offset(), std::string(tab), std::string(epsilon));
                return true;
            },
            [&is, this] <is_token _Token>(const _Token& token )
            { 
                if (is.eof())
                    return false;
                auto peek = is.peek();
                if (std::hash<Term>()(token) != peek.token) 
                    return false;
                InputToken eat;
                is >> eat;
                Logging::Log<Logging::DEBUG>("{:4}{}`{}`", eat.reference.offset(), std::string(tab), eat.reference.extract());
                return true;
            },
            []( auto&  ) 
            { 
                return false;
            }
        }, term); 

        if (!match) {
            if (!result.empty())
            {
                throw Error(std::format("Grammar is not deterministic in {}", std::string(symbol)), start.reference);
            }
            return result;
        }
    }
    InputToken end = is.peek();   // TODO: maybe optimize without symbol copies
    result.insert(result.begin(), ParsedSymbol{symbol, SourceSpan(start.reference.from, end.reference.from-start.reference.from, start.reference.source)});
    return result;
}

}
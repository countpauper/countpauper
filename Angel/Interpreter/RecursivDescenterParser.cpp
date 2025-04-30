

#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Lexer.h"
#include "Interpreter/Error.h"
#include <sstream>
#include <ranges>
#include <assert.h>

namespace Interpreter 
{
    


Parser::Parser(const Syntax& syntax) :
    syntax(syntax)
{
}

std::vector<std::string> Parser::ParseIt(const std::string_view source)
{
    TokenStream tokens;
    std::stringstream sourcestream;
    sourcestream << source;
    Lexicon lexicon(syntax);
    Lexer(lexicon).Process(sourcestream, tokens);
    SymbolStream os;
    Parse(tokens, os);
    //auto symbols =  std::ranges::views::istream<OutputSymbol>(os) | std::views::transform([](const OutputSymbol& symbol)
    auto symbols =  os.Flush() | std::views::transform([](const OutputSymbol& symbol)
    {
        return std::string(symbol.symbol);
    });

    return std::vector<std::string>(symbols.begin(), symbols.end());
}

RecursiveDescentParser::RecursiveDescentParser(const Syntax& syntax) :
    Parser(syntax)
{
}


void RecursiveDescentParser::Parse(TokenStream& is, SymbolStream& os)
{
    auto root = syntax.Start();
    auto input = is.Flush();
    assert(input.back().token == 0); // skip end
    auto it = input.begin();
    auto output = Recurse(root, it, input.end()-1);
    if (output.empty())
        throw Error("Unexpected token", it->reference);
    for(const auto& symbol : output)
    {
        os << symbol;
    }
}

std::vector<OutputSymbol> RecursiveDescentParser::Recurse(const std::string_view symbol, 
    RecursiveDescentParser::InputIterator& from, RecursiveDescentParser::InputIterator to)
{
    auto rules = syntax.Lookup(symbol);
    for(const auto& rule: rules)
    {
        // TODO: use a subrange instead of from to or perhaps std::views::istream<InputToken>
        // altough that range is also only single pass 
        std::vector<OutputSymbol> result = Recurse(rule.first, *rule.second, from, to);
        if (!result.empty())
            return result;
    }
    return std::vector<OutputSymbol>();
}


std::vector<OutputSymbol> RecursiveDescentParser::Recurse(const std::string_view name, const Terms& terms, 
    RecursiveDescentParser::InputIterator& from, RecursiveDescentParser::InputIterator to)
{
    std::vector<OutputSymbol> result;
    result.emplace_back(name, from->reference);
    auto it = from;
    for(const Term& term: terms)
    {
        if (const Symbol* subSymbol = std::get_if<Symbol>(&term))
        {
            auto subResult = Recurse(subSymbol->name, it, to); 
            if (subResult.empty())
                return subResult;
            for(const auto& i : subResult)
                result.emplace_back(i);
        }
        else if (const Literal* literal = std::get_if<Literal>(&term))
        {
            if (!literal->IsEpsilon())
            {
                if (it==to)
                    return std::vector<OutputSymbol>();
                if (std::hash<Term>()(term) != it->token) 
                    return std::vector<OutputSymbol>();
                ++it;
            }
        } 
        else if (const Regex* regex = std::get_if<Regex>(&term))    // TODO also regex
        {
            if (!regex->IsEpsilon())
            {
                if (it==to)
                    return std::vector<OutputSymbol>();
                if (std::hash<Term>()(term) != it->token) 
                    return std::vector<OutputSymbol>();
                ++it;
            }
        } 
        else 
        {
            return std::vector<OutputSymbol>();
        }
    }
    result[0].location.length = it->reference.from - from->reference.from;
    from = it;
    return result;
}

}
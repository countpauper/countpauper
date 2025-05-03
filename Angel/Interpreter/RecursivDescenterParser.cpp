

#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Lexer.h"
#include "Interpreter/Error.h"
#include "Interpreter/Utils.h"
#include <sstream>
#include <ranges>
#include <assert.h>

namespace Interpreter 
{
    


Parser::Parser(const Syntax& syntax) :
    syntax(syntax)
{
}

std::string Parser::ParseIt(const std::string_view source)
{
    TokenStream tokens;
    std::stringstream sourcestream;
    sourcestream << source;
    Lexicon lexicon(syntax);
    Lexer(lexicon).Process(sourcestream, tokens);
    SymbolStream os;
    Parse(tokens, os);
    std::string result;
    for(auto symbol : os.View())
    {
        if (symbol.location.length == 0)
            continue;
        if (!result.empty())
            result += " ";
        result += std::to_string(*lexicon[symbol.symbol]);
    }
    return result;

}

RecursiveDescentParser::RecursiveDescentParser(const Syntax& syntax) :
    Parser(syntax)
{
    if (syntax.IsLeftRecursive())
        throw std::runtime_error("Left recursion in syntax may cause infite recursion during recursive descent");
}

void RecursiveDescentParser::Parse(TokenStream& is, SymbolStream& os)
{
    auto root = syntax.Root();
    auto input = is.Dump();
    auto it = input.begin();
    assert(input.back().token == 0); // skip end
    auto output = Recurse(root, it, input.end()-1);
    if (output.empty())
        throw Error("Unexpected token", it->reference);
    for(const auto& symbol : output)
    {
        os << symbol;
    }
}

std::vector<OutputSymbol> RecursiveDescentParser::Recurse(hash_t symbol,
    RecursiveDescentParser::InputIterator& from, RecursiveDescentParser::InputIterator to)
{
    auto rules = syntax.Lookup(symbol);
    for(const auto& rule: rules)
    {
        printf("%s {\n", rule.second.symbol.c_str());
        std::vector<OutputSymbol> result = Recurse(rule.first, rule.second.terms, from, to);
        if (!result.empty()) 
        {
            printf("} %s pass\n", rule.second.symbol.c_str());
            return result;
        }
        else 
        {
            printf("} %s fail\n", rule.second.symbol.c_str());
        }
    }
    return std::vector<OutputSymbol>();
}


std::vector<OutputSymbol> RecursiveDescentParser::Recurse(hash_t symbol, const Terms& terms, 
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
                auto subResult = Recurse(std::hash<Term>()(subSymbol), it, to); 
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

        if (!match) 
            return std::vector<OutputSymbol>();
    }
    result[0].location.length = it->reference.from - from->reference.from;
    from = it;
    return result;
}

}
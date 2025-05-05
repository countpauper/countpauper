#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Error.h"

namespace Interpreter
{
class Expectation
{
public:
    explicit Expectation(Symbol s) :
        symbol(s)
    {
    }
    Symbol symbol;
};

Expectation Expect(Symbol s)
{
    return Expectation(s);
}

SymbolStream& operator>>(SymbolStream& s, const Expectation& e)
{
    OutputSymbol input;
    s >> input;
    if (e.symbol != input.symbol)
        throw Error("Unexpected symbol", input.location);

    return s;
}

Syntax GrammarGenerator::operator()(std::istream& source, SymbolStream& parse) const
{
     Syntax result;
     parse >> Expect("syntax");
     while(!parse.eof()) {
        //RuleGenerator rule;
        //parse >> rule;
        // parse >> RuleGenerator()
     }
     return result;
}

}
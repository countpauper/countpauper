#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Error.h"

namespace Interpreter
{

class Expectation
{
public:
    explicit Expectation(const std::string_view name) :
        name(name)
    {
    }
    std::string name;
};

Expectation Expect(const std::string_view s)
{
    return Expectation(s);
}

SymbolStream& operator>>(SymbolStream& is, const Expectation& e)
{
    OutputSymbol input;
    is >> input;
    if (Symbol(e.name) != input.symbol)
        throw Error(std::format("Unexpected symbol {} instead of {}", std::string(input.symbol), e.name), input.location);

    return is;
}

/*
SymbolStream& operator>>(SymbolStream& is, Rule& rule)
{
    is >> Expect("rule");
    return is;
}

SymbolStream& operator>>(SymbolStream& is, Syntax& syntax)
{
    Rule rule;
    Condition syntax_tail("syntax-tail");
    is >> Expect("syntax");
    do {
        Source rule_name(is, "rule-name");
        is >> rule_name >> Expect("expression");
        Condition expression_tail("expression-tail");
        do 
        {
            is >> Expect("list");
            Condition list_tail("list-tail")
            do 
            {
                is >> Expect("term");
                Option is_literal("rule-name", "literal");
                is >> is_literal;
                if (is_literal)
                {

                }
                else 
                {
                    Source symbol_name()
                    rule.terms.push_back(Symbol())
                }
            } while(list_tail);
        } while (expression_tail);

        syntax.emplace(rule);
        is >> syntax_tail >> Maybe("line-end")
    } while (tail);

    return is;
}

*/

Syntax GrammarGenerator::operator()(std::istream& source, SymbolStream& parse) const
{
    Syntax result;
    parse >> Expect("syntax");

    /*
    auto rules = SequenceGenerator(source, parse, "syntax", "syntax-tail", "syntax-end");
     
    Syntax result = SyntaxGenerator(source, parse);
    parse >> result;
*/
    return std::move(result);
 }

}
#include "Engine/Interpreter.h"
#include "Parser/Grammar.h"
#include "Parser/BNF.h"
#include "Parser/SymbolStream.h"
#include "Parser/Source.h"
#include "Logic/Expression.h"
#include "Parser/Logging.h"
#include <format>
#include <unistd.h>
#include <cassert>

namespace Angel::Engine
{


static Logging::Tabber tab;

Parser::Syntax ConstructSyntax(std::filesystem::path fn)
{
    Parser::Grammar bnf(Parser::BNF); 
    //char buf[1024];
    // printf("cwd: %s\n", getcwd(buf, sizeof(buf))); //std::filesystem::current_path().c_str());
    Parser::FileSource angelBNF(fn);
    return bnf.Parse(angelBNF);
}

AngelInterpreter::AngelInterpreter() :
    syntax(ConstructSyntax("angel.bnf")),
    parser(syntax)
{
}

struct ParseState 
{
    bool braces : 1 = false;
};

std::pair<Logic::Expression, ParseState> GenerateExpression(const Parser::ParsedSymbol& expressionSymbol, Parser::SymbolStream& parse, bool allowId=false);
std::vector<Logic::Expression> GenerateSequence(Parser::SymbolStream& parse);


Parser::ParsedSymbol SkipTo( Parser::SymbolStream& parse, std::string_view symbol)
{
    Parser::ParsedSymbol input; 
    while(!parse.eof())
    {    
        parse >> input;
        if (input.symbol == Parser::Symbol(symbol))
        {
            return input;
        }
    }
    throw std::runtime_error(std::format("End of file when looking for {}", symbol));
}

Logic::Expression GeneratePredicateOrId( Parser::SymbolStream& parse, bool allowId)
{
    Parser::ParsedSymbol input; 
    Logic::Id id;
    Logic::List args;
    bool hasArguments = false;
    while(!parse.eof())
    {    
        parse >> input;
        if (input.symbol == Parser::Symbol("id"))
        {
            id = Logic::Id(input.location.extract());
        }
        else if (input.symbol == Parser::Symbol("sequence"))
        {
            assert(!hasArguments);  // double arguments is weird 
            hasArguments = true;
            args = Logic::List(std::move(GenerateSequence(parse)));
        }
        else if (input.symbol == Parser::Symbol("-predicate"))
        {
            break;
        }
    }
    assert(id); // error handling needed?
    if ((hasArguments) || (!allowId))
        return Logic::Predicate(id, std::move(args));
    else
        return id;
}

Logic::Expression GenerateObject(Parser::SymbolStream& parse, bool allowId)
{
    Parser::ParsedSymbol input; 
    parse >> input; 
    if (input.symbol == Parser::Symbol("predicate"))
    {
        return GeneratePredicateOrId(parse, allowId);
    }
    else if (input.symbol == Parser::Symbol("boolean"))
    {
        return Logic::Boolean(input.location.extract());
    }
    else if (input.symbol == Parser::Symbol("positive-integer"))
    {
        return Logic::Integer(input.location.extract());
    }
    else if (input.symbol == Parser::Symbol("positive-float"))
    {
        return Logic::Real(input.location.extract());
    }
    else if (input.symbol == Parser::Symbol("variable"))
    {
        return Logic::Variable(input.location.sub(1).extract());
    }
    else if (input.symbol == Parser::Symbol("string"))
    {
        return Logic::String(input.location.sub(1,-1).extract());
    }
    else if (input.symbol == Parser::Symbol("container"))
    {    
        return GenerateObject(parse, allowId);
    }
    else if (input.symbol == Parser::Symbol("list"))
    {
        Logging::Log<Logging::DEBUG>("{}[", std::string(tab));
        auto seq = GenerateSequence(parse);
        Logging::Log<Logging::DEBUG>("{}]", std::string(tab));
        return Logic::List(std::move(seq));
    }
    else if (input.symbol == Parser::Symbol("set"))
    {
        return Logic::Set(GenerateSequence(parse));
    }
    else 
    {
        throw std::runtime_error(std::format("Unexpected object type {}", std::string(input.symbol)));
    }
}

using UnaryOperators= std::stack<Logic::Operator>;

Logic::Expression GenerateUnaryOperation(UnaryOperators& operators, unsigned minimalPrecedence, Logic::Expression&& operand)
{
    while (!operators.empty())
    {
        auto op = operators.top();
        if (op.Precedence() < minimalPrecedence)
            break;
        operators.pop();
        const auto* id = operand.GetIf<Logic::Id>();
        if (op==Logic::PrefixOperator(L'∀') && id)
            operand = Logic::All(std::string(*id));
        else 
            operand = Logic::Expression(op, { std::move(operand) });
    }
    return operand;
}

std::pair<Logic::Expression, ParseState> GenerateValue(Parser::SymbolStream& parse, bool allowId)
{
    Parser::ParsedSymbol input;
    UnaryOperators unary_ops;
    while(!parse.eof())
    {      
        parse >> input;
        if (input.symbol == Parser::Symbol("prefix-operator"))
        {
            unary_ops.push(Logic::Operator::Find<Logic::PrefixOperator, Logic::Filter>(
                                                                input.location.extract()));
        }        
        if (input.symbol == Parser::Symbol("braced-expression"))
        {
            Logic::Expression expression = GenerateExpression(SkipTo(parse, "expression"), parse, allowId).first;
            expression = GenerateUnaryOperation(unary_ops, 0, std::move(expression));
            return std::make_pair(expression, ParseState{.braces=true});
        }
        else if (input.symbol == Parser::Symbol("object"))
        {   
            Logic::Expression expression = GenerateObject(parse, allowId);
            expression = GenerateUnaryOperation(unary_ops, 0, std::move(expression));
            return std::make_pair(expression,ParseState{});
        }
    }
    throw std::runtime_error("Unexpected end of file when looking for value");
}

Logic::Expression GenerateOperation(Logic::Operator op, Logic::Expression&& lhs, ParseState lhstate, Parser::SymbolStream& parse, bool allowId)
{
    auto [rhs, rhstate] = GenerateValue(parse, allowId);
    if (auto* operation = lhs.GetIf<Logic::Operation>())
    {
        if (operation->GetOperator() == op && op.Operands() > 2)
        {
            Logging::Log<Logging::DEBUG>("{} Append {}  {}  {}", 
                std::string(tab),
                Logic::to_string(*operation),
                std::string(op), 
                Logic::to_string(rhs));
            operation->AddRight(std::move(rhs));
            return *operation;;
        }
        else if (lhstate.braces)
        {
            Logging::Log<Logging::DEBUG>("{} LHS braced: ({})  {}  {} ", 
                std::string(tab),
                Logic::to_string(lhs), std::string(op), Logic::to_string(rhs));
        }
        else if (rhstate.braces)
        {
            Logging::Log<Logging::DEBUG>("{} RHS braced: {}  {}  ({}) ", 
                std::string(tab),
                Logic::to_string(lhs), std::string(op), Logic::to_string(rhs));
        }
        else if (operation->GetOperator().Precedence() < op.Precedence())
        {
            auto adoptedOperand = operation->RemoveRight();
            auto highPrecedenceOperation = Logic::Expression(op, {std::move(adoptedOperand), std::move(rhs)});
            Logging::Log<Logging::DEBUG>("{} LHS Op {} < {}. Substitute {} before {}", 
                std::string(tab),
                std::string(operation->GetOperator()), 
                std::string(op), 
                Logic::to_string(highPrecedenceOperation),
                Logic::to_string(*operation));
            operation->AddRight(std::move(highPrecedenceOperation));
            return std::move(lhs);
        }
        else 
        {
            Logging::Log<Logging::DEBUG>("{} LHS Op {} >= {}. Nest {} in {}", 
                std::string(tab),
                std::string(operation->GetOperator()), 
                std::string(op), 
                Logic::to_string(*operation),
                Logic::to_string(rhs));
        }
    }
    else 
    {
        Logging::Log<Logging::DEBUG>("{} RHS is a value: new {}  {}  {} ", 
            std::string(tab),            
            Logic::to_string(lhs), std::string(op), Logic::to_string(rhs));
    }
    return Logic::Expression(op, {std::move(lhs), std::move(rhs)});
}


std::pair<Logic::Expression, ParseState> GenerateExpression(const Parser::ParsedSymbol& expressionSymbol, Parser::SymbolStream& parse, bool allowId)
{
    Parser::ParsedSymbol input;
    Logic::Expression expression;
    ParseState state;
    ++tab;
    while(!parse.eof())
    {   
        parse >> input;
        if (input.symbol == Parser::Symbol("binary-operator"))
        {
            auto operatorTag = input.location.extract();
            Logic::Operator op = Logic::Operator::Find<Logic::BinaryOperator, Logic::MultiOperator>(
                                                                operatorTag);
            if (!op)
                throw std::runtime_error(std::format("Unknown operator {}", operatorTag));
            if (!expression)
                throw std::runtime_error(std::format("Unexpected operator {} must follow operand", operatorTag));

            expression = GenerateOperation(op, std::move(expression), state, parse, allowId);
            state.braces = false;
        }
        if (input.symbol == Parser::Symbol("prefix-operator"))
        {
            assert(false);  // these should be in GenerateValue now 
        }
        if (input.symbol == Parser::Symbol("value"))
        {
            auto [vexpression, vstate] = GenerateValue(parse, allowId);
            expression = std::move(vexpression);
            state = std::move(vstate);
        }
        else if (input.symbol == Parser::Symbol("-expression"))
        {
            assert(expression);
            Logging::Log<Logging::DEBUG>("{}`{}` = {}", std::string(tab), expressionSymbol.location.extract(), Logic::to_string(expression));
            break;
        }
        assert(input.symbol != Parser::Symbol("sequence"));
        assert(input.symbol != Parser::Symbol("-sequence"));
    }
    
    --tab;
    return std::make_pair(std::move(expression), state);
}

std::vector<Logic::Expression> GenerateSequence(Parser::SymbolStream& parse)
{
    std::vector<Logic::Expression> result;
    Parser::ParsedSymbol input;
    while(!parse.eof())
    {      
        parse >> input;
        if (input.symbol == Parser::Symbol("expression"))
        {
            result.emplace_back(GenerateExpression(input, parse, true).first);
        }
        else if (input.symbol == Parser::Symbol("-sequence"))
        {
            break;
        }
    }
    return result;
}


void GenerateAxiom(Parser::SymbolStream& parse, Logic::Knowledge& knowledge)
{
    Parser::ParsedSymbol input; 
    Logic::Predicate predicate;
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Parser::Symbol("predicate"))
        {
            assert(!predicate && "Generate predicated was not moved to knowledge");
            auto expression = GeneratePredicateOrId(parse, false);
            predicate = expression.Get<Logic::Predicate>();
        }    
        else if (input.symbol == Parser::Symbol("axioms") || 
                 input.symbol == Parser::Symbol("-axioms"))
        {
            if (predicate)
            {
                assert(!tab);
                Logging::Log<Logging::INFO>("> {}", Logic::to_string(predicate));
                knowledge.Know(std::move(predicate));
            }
            return;
        }
        else if (input.symbol == Parser::Symbol("expression"))
        {
            Logic::Expression terms = GenerateExpression(input, parse, true).first;
            Logic::Association clause{std::move(predicate), std::move(terms)};
            assert(!tab);
            Logging::Log<Logging::INFO>("# {}", Logic::to_string(clause));
            knowledge.Know(std::move(clause));
            return;
        }
    }
}

std::size_t GenerateKnowledge(const Parser::Source& source, Parser::SymbolStream& parse, Logic::Knowledge& knowledge)
{
    Parser::ParsedSymbol input; 
    std::size_t count = 0;
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Parser::Symbol("knowledge"))
        {
            auto trailing = source.span(input.location.size());

            if (!trailing.empty())
            {
                throw std::runtime_error(std::format("Trailing input at {}: {}",
                    std::string(trailing), 
                    trailing.extract()));
            }
        }
        if (input.symbol == Parser::Symbol("axiom"))
        {
            GenerateAxiom(parse, knowledge);
            ++count;
        }
    }
    return count;
}

template<std::ostream* log>
void LogParse(Parser::SymbolStream copy)
{
    Logging::Tabber parsetab;
    Parser::ParsedSymbol input; 
    std::stack<Parser::SourceSpan> spans;
    while(!copy.eof())
    {
        copy >> input;
        if (spans.empty())
        {
            parsetab = 0;
        }
        while(!spans.empty() && !input.location.in(spans.top())) 
        {
            spans.pop();
            --parsetab;
        }
        spans.push(input.location);
        ++parsetab;
        if (input.location.empty())
        {
            Logging::Log<log>("{}{}",std::string(parsetab), std::string(input.symbol));
        }
        else 
        {
            Logging::Log<log>("{}{} {} [{}..{}]", 
                std::string(parsetab), std::string(input.symbol), 
                input.location.extract(), input.location.from, input.location.from + input.location.length-1);
        }
    }   
}

std::size_t AngelInterpreter::Interpret(const Parser::Source& source, Logic::Knowledge& knowledge )
{
    Parser::SymbolStream os;
    parser.Parse(source, os);
    tab = 0;
    LogParse<Logging::OFF>(os);
    return GenerateKnowledge(source, os, knowledge);
}    

Logic::Expression AngelInterpreter::InterpretQuery(const ::Parser::Source& source)
{
    Parser::SymbolStream os;
    parser.Parse(source, os, "query"); 
    tab = 0;
    return GenerateExpression(SkipTo(os, "expression"), os).first;
}

}
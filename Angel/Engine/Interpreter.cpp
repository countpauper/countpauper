#include "Engine/Interpreter.h"
#include "Interpreter/Grammar.h"
#include "Interpreter/BNF.h"
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Source.h"
#include "Logic/Expression.h"
#include "Interpreter/Logging.h"
#include <format>
#include <unistd.h>
#include <cassert>

namespace Angel::Engine
{

Interpreter::Syntax ConstructSyntax(std::filesystem::path fn)
{
    Interpreter::Grammar bnf(Interpreter::BNF); 
    //char buf[1024];
    // printf("cwd: %s\n", getcwd(buf, sizeof(buf))); //std::filesystem::current_path().c_str());
    Interpreter::FileSource angelBNF(fn);
    return bnf.Parse(angelBNF);
}

AngelInterpreter::AngelInterpreter() :
    syntax(ConstructSyntax("angel.bnf")),
    parser(syntax)
{
}

Logic::Expression GenerateExpression(Interpreter::SymbolStream& parse, bool allowId=false);
std::vector<Logic::Expression> GenerateSequence(Interpreter::SymbolStream& parse);

Logic::Expression GeneratePredicateOrId( Interpreter::SymbolStream& parse, bool allowId)
{
    Interpreter::ParsedSymbol input; 
    Logic::Id id;
    Logic::List args;
    bool hasArguments = false;
    while(!parse.eof())
    {    
        parse >> input;
        if (input.symbol == Interpreter::Symbol("id"))
        {
            id = Logic::Id(input.location.extract());
        }
        else if (input.symbol == Interpreter::Symbol("sequence"))
        {
            assert(!hasArguments);  // double arguments is weird 
            hasArguments = true;
            args = Logic::List(std::move(GenerateSequence(parse)));
        }
        else if (input.symbol == Interpreter::Symbol("-predicate"))
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

Logic::Expression GenerateObject(Interpreter::SymbolStream& parse, bool allowId)
{
    Interpreter::ParsedSymbol input; 
    parse >> input; 
    if (input.symbol == Interpreter::Symbol("predicate"))
    {
        return GeneratePredicateOrId(parse, allowId);
    }
    else if (input.symbol == Interpreter::Symbol("boolean"))
    {
        return Logic::Boolean(input.location.extract());
    }
    else if (input.symbol == Interpreter::Symbol("positive-integer"))
    {
        return Logic::Integer(input.location.extract());
    }
    else if (input.symbol == Interpreter::Symbol("variable"))
    {
        return Logic::Variable(input.location.sub(1).extract());
    }
    else if (input.symbol == Interpreter::Symbol("string"))
    {
        return Logic::String(input.location.sub(1,-1).extract());
    }
    else if (input.symbol == Interpreter::Symbol("container"))
    {    
        return GenerateObject(parse, allowId);
    }
    else if (input.symbol == Interpreter::Symbol("list"))
    {
        return Logic::List(GenerateSequence(parse));
    }
    else if (input.symbol == Interpreter::Symbol("set"))
    {
        return Logic::Set(GenerateSequence(parse));
    }
    else 
    {
        throw std::runtime_error(std::format("Unexpected object type {}", std::string(input.symbol)));
    }
}

Logic::Expression GenerateExpression(Interpreter::SymbolStream& parse, bool allowId)
{
    Interpreter::ParsedSymbol input;
    Logic::Tuple operands;
    Logic::Operator ope;
    std::stack<Logic::Operator> unary_ops;
    while(!parse.eof())
    {   
        parse >> input;
        if (input.symbol == Interpreter::Symbol("binary-operator"))
        {
            // TODO if new operator is different, determine precedence. 
            // if higher: eg a+ b * ... 
            //    the last object is the start of a new operations 
            //    this operation is appended to the existing operation as its new last element 
            // if lower:   a*b + ...
            //    the last object is appended to the existing operation
            //    that operation is the first argument of the new one for that operator
            // if the same: eg a/b * c same as lower  
            //  
            auto operatorTag = input.location.extract();
            Logic::Operator nextOperator = Logic::Operator::Find<Logic::BinaryOperator, Logic::MultiOperator>(
                                                                operatorTag);
            if (!nextOperator)
                throw std::runtime_error(std::format("Unknown operator {}", operatorTag));
            assert(!ope || nextOperator == ope); // not yet implemented 
            ope = nextOperator;
        }
        if (input.symbol == Interpreter::Symbol("prefix-operator"))
        {
            unary_ops.push(Logic::Operator::Find<Logic::PrefixOperator, Logic::Filter>(
                                                                input.location.extract()));
        }
        else if (input.symbol == Interpreter::Symbol("object"))
        {   // TODO: prefixed values and braces
            Logic::Expression newOparand = GenerateObject(parse, allowId);
            while (!unary_ops.empty())
            {
                auto op = unary_ops.top();
                unary_ops.pop();
                const auto* id = newOparand.GetIf<Logic::Id>();
                if (op==Logic::PrefixOperator(L'∀') && id)
                    newOparand = Logic::All(std::string(*id));
                else 
                    newOparand = Logic::Expression(op, { newOparand });
            }
            operands.emplace_back(std::move(newOparand));
        }
        else if (input.symbol == Interpreter::Symbol("-expression"))
        {
            return Logic::Expression(ope, std::move(operands));
        }

    }
    return Logic::Expression(ope, std::move(operands));
}

std::vector<Logic::Expression> GenerateSequence(Interpreter::SymbolStream& parse)
{
    std::vector<Logic::Expression> result;
    Interpreter::ParsedSymbol input;
    while(!parse.eof())
    {      
        parse >> input;
        if (input.symbol == Interpreter::Symbol("expression"))
        {
            result.push_back(GenerateExpression(parse, true));
        }
        else if (input.symbol == Interpreter::Symbol("-sequence"))
        {
            break;
        }
    }
    return result;
}


void GenerateAxiom(Interpreter::SymbolStream& parse, Logic::Knowledge& knowledge)
{
    Interpreter::ParsedSymbol input; 
    Logic::Predicate predicate;
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Interpreter::Symbol("predicate"))
        {
            assert(!predicate && "Generate predicated was not moved to knowledge");
            auto expression = GeneratePredicateOrId(parse, false);
            predicate = expression.Get<Logic::Predicate>();
        }    
        else if (input.symbol == Interpreter::Symbol("axioms") || 
                 input.symbol == Interpreter::Symbol("-axioms"))
        {
            if (predicate)
            {
                Logging::Log<Logging::INFO>("> {}", Logic::to_string(predicate));
                knowledge.Know(std::move(predicate));
            }
            return;
        }
        else if (input.symbol == Interpreter::Symbol("expression"))
        {
            Logic::Expression terms = GenerateExpression(parse, true);
            Logic::Association clause{std::move(predicate), std::move(terms)};
            Logging::Log<Logging::INFO>("> {}", Logic::to_string(clause));
            knowledge.Know(std::move(clause));
            return;
        }
    }
}

std::size_t GenerateKnowledge(const Interpreter::Source& source, Interpreter::SymbolStream& parse, Logic::Knowledge& knowledge)
{
    Interpreter::ParsedSymbol input; 
    std::size_t count = 0;
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Interpreter::Symbol("knowledge"))
        {
            auto trailing = source.span(input.location.size());

            if (!trailing.empty())
            {
                throw std::runtime_error(std::format("Trailing input at {}: {}",
                    std::string(trailing), 
                    trailing.extract()));
            }
        }
        if (input.symbol == Interpreter::Symbol("axiom"))
        {
            GenerateAxiom(parse, knowledge);
            ++count;
        }
    }
    return count;
}

std::size_t AngelInterpreter::Interpret(const Interpreter::Source& source, Logic::Knowledge& knowledge )
{
    Interpreter::SymbolStream os;
    parser.Parse(source, os);
    return GenerateKnowledge(source, os, knowledge);
}    

Logic::Expression AngelInterpreter::InterpretQuery(const ::Interpreter::Source& source)
{
    Interpreter::SymbolStream os;
    parser.Parse(source, os, "query"); 
    return GenerateExpression(os);
}

}
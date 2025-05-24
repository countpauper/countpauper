#include "Engine/Interpreter.h"
#include "Interpreter/Grammar.h"
#include "Interpreter/BNF.h"
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Source.h"
#include "Logic/Expression.h"
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
    // TODO: AngelInterpreter tests are ran in Angel/build/Engine/unittest from testmate in vscode
    // this relative path makes it fail if ran from another cwd
    // figure out how to inject the location while still keeping this file as the source
    syntax(ConstructSyntax("angel.bnf")),
    parser(syntax)
{
}

Logic::Expression GenerateExpression(Interpreter::SymbolStream& parse, bool allowId=false);


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
            hasArguments = true;
        }
        else if (input.symbol == Interpreter::Symbol("expression"))
        {
            hasArguments = true;
            args.push_back(GenerateExpression(parse, true));
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
    else 
    {
        throw std::runtime_error(std::format("Unexpected object type {}", std::string(input.symbol)));
    }
}

Logic::Expression GenerateExpression(Interpreter::SymbolStream& parse, bool allowId)
{
    Interpreter::ParsedSymbol input;
    Logic::Collection operands;
    Logic::BinaryOperator ope;
    std::stack<Logic::UnaryOperator> unary_ops;
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
            Logic::BinaryOperator nextOperator{input.location.extract()};
            assert(!ope || nextOperator == ope); // not yet implemented 
            ope = nextOperator;
        }
        if (input.symbol == Interpreter::Symbol("unary-operator"))
        {
            unary_ops.push(Logic::UnaryOperator(input.location.extract()));
        }
        else if (input.symbol == Interpreter::Symbol("object"))
        {   // TODO: prefixed values and braces
            Logic::Expression newOparand = GenerateObject(parse, allowId);
            while (!unary_ops.empty())
            {
                newOparand = Logic::Expression(unary_ops.top(), { newOparand });
                unary_ops.pop();
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
                knowledge.Know(std::move(predicate));
            return;
        }
        else if (input.symbol == Interpreter::Symbol("expression"))
        {
            Logic::Expression terms = GenerateExpression(parse);
            knowledge.Know(Logic::Association{std::move(predicate), std::move(terms)});
            return;
        }
    }
}

void GenerateKnowledge(const Interpreter::Source& source, Interpreter::SymbolStream& parse, Logic::Knowledge& knowledge)
{
    Interpreter::ParsedSymbol input; 
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Interpreter::Symbol("axiom"))
        {
            GenerateAxiom(parse, knowledge);
        }
    }
}

void AngelInterpreter::Interpret(const Interpreter::Source& source, Logic::Knowledge& knowledge )
{
    Interpreter::SymbolStream os;
    parser.Parse(source, os);
    GenerateKnowledge(source, os, knowledge);
}    

Logic::Expression AngelInterpreter::InterpretQuery(const ::Interpreter::Source& source)
{
    Interpreter::SymbolStream os;
    parser.Parse(source, os, "query");  // TODO parse expression instead of single predicate
    return GenerateExpression(os);
}

}
#include "Engine/Interpreter.h"
#include "Interpreter/Grammar.h"
#include "Interpreter/BNF.h"
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Source.h"
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

Logic::Predicate GeneratePredicate( Interpreter::SymbolStream& parse)
{
    Interpreter::ParsedSymbol input; 
    Logic::Id id;
    while(!parse.eof())
    {    
        parse >> input;
        if (input.symbol == Interpreter::Symbol("id"))
        {
            id = Logic::Id(input.location.extract());
        }
        else if (input.symbol == Interpreter::Symbol("-terms"))
        {
            break;
        }
    }
    assert(id); // error handling needed?
    return Logic::Predicate(id);
}

enum class Operator
{
    none,
    access,
    member,
    power,
    multiplication,
    division,
    summation, 
    subtraction,
    conjunction,
    disjunction,
    equality,
    inequality,
    greater,
    lower,
    greater_equal,
    lower_equal
};


Logic::Object GenerateObject(Interpreter::SymbolStream& parse)
{
    Interpreter::ParsedSymbol input; 
    parse >> input; 
    if (input.symbol == Interpreter::Symbol("predicate"))
    {
        return GeneratePredicate(parse);
    }
    else if (input.symbol == Interpreter::Symbol("boolean"))
    {
        return Logic::Boolean(input.location.extract());
    }
    else if (input.symbol == Interpreter::Symbol("integer"))
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

Logic::Expression GenerateExpression(Interpreter::SymbolStream& parse)
{
    std::vector<std::pair<Operator, Logic::Object>> operands; 
    Interpreter::ParsedSymbol input; 
    while(!parse.eof())
    {   
        parse >> input;
        if (input.symbol == Interpreter::Symbol("object"))
        {   // TODO: prefixed values and braces
            operands.push_back(std::make_pair(Operator::none, GenerateObject(parse)));
        }
        else if (input.symbol == Interpreter::Symbol("-expression"))
        {
            return operands.front().second;
        }
    }
    return operands.front().second;
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
            predicate = GeneratePredicate(parse);
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
            knowledge.Know(std::move(predicate), std::move(terms));
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

Logic::Expression AngelInterpreter::InterpretExpression(const ::Interpreter::Source& source)
{
    Interpreter::SymbolStream os;
    parser.Parse(source, os, "expression");  // TODO parse expression instead of single predicate
    Interpreter::ParsedSymbol startSymbol;
    os >> startSymbol;
    assert(startSymbol.symbol == Interpreter::Symbol("expression"));  // error handling needed?
    return GenerateExpression(os);
}

}
#include "Engine/Interpreter.h"
#include "Interpreter/Grammar.h"
#include "Interpreter/BNF.h"
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Source.h"
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
    }
    assert(id); // error handling needed?
    return Logic::Predicate(id);
}

void GenerateKnowledge(const Interpreter::Source& source, Interpreter::SymbolStream& parse, Logic::Knowledge& knowledge)
{
    Interpreter::ParsedSymbol input; 
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Interpreter::Symbol("predicate"))
        {
            knowledge.Know(GeneratePredicate(parse));
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
    parser.Parse(source, os, "predicate");  // TODO parse expression instead of single predicate
    Interpreter::ParsedSymbol startSymbol;
    os >> startSymbol;
    assert(startSymbol.symbol == Interpreter::Symbol("predicate"));  // error handling needed?
    return GeneratePredicate(os);
}

}
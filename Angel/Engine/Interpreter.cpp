#include "Engine/Interpreter.h"
#include "Interpreter/Grammar.h"
#include "Interpreter/BNF.h"
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Source.h"
#include <unistd.h>

namespace Angel::Engine
{

Interpreter::Syntax ConstructSyntax(std::filesystem::path fn)
{
    Interpreter::Grammar bnf(Interpreter::BNF); 
    //char buf[1024];
    //printf("cwd: %s", getcwd(buf, sizeof(buf))); //std::filesystem::current_path().c_str());
    Interpreter::FileSource angelBNF(fn);
    return bnf.Parse(angelBNF);
}

AngelInterpreter::AngelInterpreter() :
    // TODO: AngelInterpreter tests are ran in Angel/build/Engine/unittest from testmate in vscode
    // this relative path makes it fail if ran from another cwd
    // figure out how to inject the location while still keeping this file as the source
    syntax(ConstructSyntax("../../../Engine/angel.bnf")),
    parser(syntax)
{
}

void GenerateKnowledge(const Interpreter::Source& source, Interpreter::SymbolStream& parse, Logic::Knowledge& knowledge)
{
    Interpreter::ParsedSymbol input; 
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Interpreter::Symbol("predicate"))
        {
            knowledge.Know(Logic::Predicate(input.location.extract()));
        }
    }
}

std::vector<Logic::Predicate> AngelInterpreter::Interpret(const Interpreter::Source& source, Logic::Knowledge& knowledge )
{
    std::vector<Logic::Predicate> queries;
    // TODO: this can be a constructor of the engine interpreter 
    Interpreter::SymbolStream os;
    parser.Parse(source, os);
    GenerateKnowledge(source, os, knowledge);
    return queries;
}    
}
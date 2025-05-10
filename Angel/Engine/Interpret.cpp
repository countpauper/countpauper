#include "Engine/Interpret.h"
#include "Interpreter/Grammar.h"
#include "Interpreter/BNF.h"
#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/SymbolStream.h"
#include <unistd.h>

namespace Angel::Engine
{


void GenerateKnowledge(Interpreter::Source& source, Interpreter::SymbolStream& parse, Logic::Knowledge& knowledge)
{
    source.clear();
    source.exceptions(source.badbit|source.failbit|source.eofbit);
    Interpreter::ParsedSymbol input; 
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Interpreter::Symbol("predicate"))
        {
            knowledge.Know(Logic::predicate(input.location.extract(source)));
        }
    }
}

std::vector<Logic::Predicate> Interpret(Interpreter::Source& source, Logic::Knowledge& knowledge )
{
    std::vector<Logic::Predicate> queries;
    // TODO: this can be a constructor of the engine interpreter 
    Interpreter::Grammar bnf(Interpreter::BNF); 
    char buf[1024];
    printf("cwd: %s", getcwd(buf, sizeof(buf))); //std::filesystem::current_path().c_str());
    Interpreter::FileSource angelBNF(std::filesystem::path("../../../Engine/angel.bnf"));
    Interpreter::Syntax angelSyntax = bnf.Parse(angelBNF);
    Interpreter::RecursiveDescentParser angel(angelSyntax);

    Interpreter::SymbolStream os;
    angel.Parse(source, os);

    GenerateKnowledge(source, os, knowledge);

    return queries;
}    
}
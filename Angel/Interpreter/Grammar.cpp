#include "Interpreter/Grammar.h"
#include "Interpreter/Lexer.h"
#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/RecursiveDescentParser.h"
namespace Interpreter 
{

Grammar::Grammar(const Interpreter::Syntax& form) :
    form(form)
{

}

Syntax Grammar::Parse(const Source& src)
{
    RecursiveDescentParser parser(form);
    SymbolStream symbols;
    parser.Parse(src, symbols);
    return GenerateGrammar(src, symbols);
}

   

}
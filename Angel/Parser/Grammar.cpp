#include "Parser/Grammar.h"
#include "Parser/Lexer.h"
#include "Parser/GrammarGenerator.h"
#include "Parser/RecursiveDescentParser.h"
namespace Parser 
{

Grammar::Grammar(const Syntax& form) :
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
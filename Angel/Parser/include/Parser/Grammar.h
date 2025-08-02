#pragma once 
#include "Parser/Syntax.h"
#include "Parser/Source.h"

namespace Parser 
{

class Grammar
{
public:
    explicit Grammar(const Syntax& form);
    Syntax Parse(const Source& src);
private:
    const Syntax& form;
};
   
}
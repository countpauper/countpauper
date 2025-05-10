#pragma once 
#include "Interpreter/Syntax.h"
#include "Interpreter/Source.h"

namespace Interpreter 
{

class Grammar
{
public:
    explicit Grammar(const Syntax& form);
    Syntax Parse(Source& src);
private:
    const Syntax& form;
};
   
}
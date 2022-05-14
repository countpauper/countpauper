#pragma once
#include "Expression.h"

namespace Angel
{
namespace Logic
{

// An element is a sub category of Expression that is not a compound of other Expressions
// For instance an Integer, Boolean is an Element
class Element : public Expression
{
public:
};

}
}

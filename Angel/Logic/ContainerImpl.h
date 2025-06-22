#pragma once 
#include <functional>

namespace Angel::Logic 
{
class Expression;

unsigned AddToContainer(Expression&& item, std::function<unsigned(Expression&&)> addOne);

}
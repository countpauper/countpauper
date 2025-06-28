#pragma once 
#include <functional>
#include "Logic/Internal/container_iterator.h"

namespace Angel::Logic 
{
class Expression;

unsigned AddToContainer(const_container_iterator& at, Expression&& item, std::function<unsigned(const_container_iterator&, Expression&&)> addOne);

}
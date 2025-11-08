#pragma once 
#include "expectation.hpp"
namespace eul 
{


class timer_itf 
{
public:
    virtual expectation Start() = 0; 
    virtual float Time() const = 0;
};

}
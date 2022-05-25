#pragma once
#include <string>
#include "Logic/Knowledge.h"
#include <ios>

namespace Angel::Parser
{

    class Exception : public std::runtime_error
    {
    public:
        Exception(const std::string& msg) :
            std::runtime_error(msg)
        {
        }

    };

    class SyntaxError : public Exception
    {
    public:
        SyntaxError(const std::string& msg) :
            Exception(msg)
        {
        }
    };
}

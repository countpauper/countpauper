#pragma once
#include <string>
#include "Logic/Knowledge.h"
#include <ios>

namespace Angel
{
namespace Logic { class Object;  }
namespace Parser
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

Logic::Knowledge Parse(const std::wstring& text);
std::wostream& operator<<(std::wostream& s, const Logic::Object& o);
std::wistream& operator>>(std::wistream& s, Logic::Object& o);
}
}
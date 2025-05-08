#pragma once 
#include <sstream>

namespace Interpreter 
{

class Source : public std::istringstream
{
public:
    Source(const std::string_view data) :
        std::istringstream(std::string(data))
    {
    }
};

}
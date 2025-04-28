#pragma once 

#include <exception>
#include <string>
#include "Interpreter/SourceSpan.h"

namespace Interpreter 
{

class Error : public std::exception
{
public:
    Error(const std::string_view description, SourceSpan location);
    const char* what() const noexcept override;
    SourceSpan Location() const;
private:
    std::string description;
    SourceSpan location;
};

}
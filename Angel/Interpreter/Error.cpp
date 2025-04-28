#include "Interpreter/Error.h"

namespace Interpreter 
{
Error::Error(const std::string_view description, SourceSpan location) :
    description(description),
    location(location)
{
}

const char* Error::what() const noexcept
{
    return description.c_str();
}


SourceSpan Error::Location() const
{
    return location;
}

}
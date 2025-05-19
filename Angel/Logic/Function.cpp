#include "Logic/Function.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

Function::Function(Callback cb, const std::string_view doc) :
    documentation(doc),
    callback(cb)
{
    if (!callback)
        throw std::invalid_argument("Function should be implemented with a callback");

    if (documentation.empty())
        throw std::invalid_argument("Function documentation should at least include a short description");
}

Match Function::Matches(const Expression& expression, const Variables& vars) const
{
    return NoMatch; 
}

Expression Function::Infer(const class Knowledge& k, const Variables& substitutions) const
{
    return callback(k, substitutions);
}

Function::operator bool() const
{
    return bool(callback);
}

bool Function::operator==(const Function& other) const
{
    if (&callback.target_type() != &other.callback.target_type())
        return false;
    if (!callback)
        return !bool(other.callback);
    return true;
}

std::string_view Function::Documentation() const
{
    return documentation;
}

std::string_view Function::ShortDescription() const 
{
    auto eol = documentation.find('\n');
    if (eol == documentation.npos)
        return documentation;
    else 
        return std::string_view(documentation.data(), eol);
}

std::size_t Function::Hash() const
{
    std::hash<std::string_view> hash;
    return hash(ShortDescription());
}

std::ostream& operator<<(std::ostream& os, const Function& fn)
{
    os << fn.ShortDescription();
    return os;
}
}
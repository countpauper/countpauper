#include "Logic/Variable.h"
#include "Logic/Boolean.h"
#include "Logic/Expression.h"
#include <iostream>

namespace Angel::Logic
{

Variable::Variable(const std::string_view name) :
	name(name)
{
}

Variable::operator bool() const
{
    return !name.empty();
}

bool Variable::operator==(const Variable& var) const
{
	return name == var.name;
}

std::size_t Variable::Hash() const
{
    std::hash<std::string> hasher;
    return hasher(name);
}

std::string_view Variable::Name() const
{
    return name;
}

Expression Variable::Infer(const class Knowledge& k, const Variables& substitutions) const
{
    auto it = substitutions.find(name);
    if (it!=substitutions.end())
        return it->second.Infer(k, substitutions);
    return Boolean(true);
}

Match Variable::Matches(const Expression& e, const Variables& variables) const
{
    auto it = variables.find(name);
    if (it==variables.end())
        return Variables{{name, e}};
    return it->second.Matches(e, variables);
}

std::ostream& operator<<(std::ostream& os, const Variable& id)
{
    os << "$" << id.name.c_str();
    return os;
}

}


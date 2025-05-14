#include "Logic/Variable.h"
#include "Logic/Boolean.h"
#include "Logic/Object.h"
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

Object Variable::Infer(const class Knowledge& k, const Variables& substitutions) const
{
    auto it = substitutions.find(name);
    if (it!=substitutions.end())
        return it->second;
    return Boolean(true);
}

Match Variable::Matches(const Object& o, const Variables& variables) const
{
    auto it = variables.find(name);
    if (it==variables.end())
        return Variables{{name, o}};
    return it->second.Matches(o, variables);
}

/*
Object Variable::Copy() const
{
    return var(name);
}

Match Variable::Matching(const Expression& other, const Variables& substitutions) const
{
    auto it = substitutions.find(name);
    if (it!=substitutions.end())
    {
        return it->second->Matching(other, substitutions); 
    }
    else 
    {
        return Match(Variables{{name, Object(other.Copy())}});
    }
}

const Object* Variable::Condition() const
{
    return nullptr;
}

Object Variable::Infer(const Knowledge& known, const Variables& substitutions) const
{
    auto it = substitutions.find(name);
    if (it!=substitutions.end())
    {
        return it->second.Infer(known, substitutions);
    }
    else 
    {
        return boolean(false);
    }
}

Object Variable::Cast(const std::type_info& t, const Knowledge& k) const
{
    throw CastException<Variable>(t);
}
*/


std::ostream& operator<<(std::ostream& os, const Variable& id)
{
    os << id.name.c_str();
    return os;
}

/*
Object var(const std::string_view name)
{
	return Create<Variable>(name);
}
*/

}


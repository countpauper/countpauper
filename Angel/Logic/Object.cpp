#include "Logic/Object.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Expression.h"
#include <sstream>

namespace Angel::Logic
{
    
/*
Object::Object(const Object& other) :
    Object(other->Copy())
{
}

Object& Object::operator=(const Object& other)
{
    expr.reset();
    expr = std::move(other.expr->Copy().expr);
    return *this;
}

Object::Object(Object&& other) :
    expr(std::move(other.expr))
{
}

Object& Object::operator=(Object&& other)
{
    expr = std::move(other.expr);
	return *this;
}

Object::operator bool() const
{
	if (null()) 
		return false;
	return bool(*expr);
}

bool Object::null() const 
{
	return !expr.get();
}

bool Object::operator==(const Object& other) const
{
	return expr->operator==(*other);
}

Object Object::Infer(const Knowledge& knowledge, const Variables& substitutions) const
{
    return expr->Infer(knowledge, substitutions);
}

const Expression& Object::operator*() const
{
    return *expr;
}

const Expression* Object::operator->() const
{
    return expr.get();
}

size_t Object::Hash() const
{
	if (null())
		return 0;
	else 
	{
		return std::hash<Angel::Logic::Expression>()(*expr);
	}
}

Object Object::Cast(const std::type_info& t, const Knowledge& knowledge) const
{
    return expr->Cast(t, knowledge);
}

std::ostream& operator<<(std::ostream& os, const Object& o)
{
    if (o.null())
        os << "null";
    else 
        os << *o;
    return os;
}

Object::operator std::string() const
{
    std::stringstream s;
    s << *this;
    return s.str();
}

*/
List::List(std::initializer_list<Node> listItems) :
    std::vector<Node>(listItems)
{
}

bool List::operator==(const List& rhs) const
{
    if (size()!=rhs.size())
        return false;
    return std::equal(begin(), end(), rhs.begin());
}

std::size_t List::Hash() const
{
    std::size_t result = typeid(decltype(*this)).hash_code();
    std::hash<Node> hasher;
    for(const auto& n: *this)
    {
        result ^= hasher(n);
    }
    return result;
}

bool Node::operator<(const Node&o) const
{
    std::hash<Node> hasher; 
    return hasher(*this) < hasher(o);
}

bool Node::operator==(const Node& o) const
{
    return value == o.value;
}
}


namespace std
{

size_t hash<Angel::Logic::Node>::operator()(const Angel::Logic::Node& n) const
{
    std::hash<Angel::Logic::Object> hasher;
    return hasher(n.value);
}

size_t hash<Angel::Logic::Object>::operator()(const Angel::Logic::Object& o) const
{
    return std::visit([](const auto& obj) { return obj.Hash(); }, o);
}


}

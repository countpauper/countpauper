#include "Logic/Object.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"

namespace Angel
{
namespace Logic
{



Object::Object(const std::string& tag)
{
	if (tag.empty())
	{
		return;
	}
	else if (auto boolean = Boolean::Parse(tag))
	{
        expr = std::make_unique<Boolean>(*boolean);
	}
	else if (auto integer = Integer::Parse(tag))
	{
        expr = std::make_unique<Integer>(*integer);
	}
	else
	{
        expr = std::make_unique<Id>(tag);
	}
}

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
	return Match(*other);
}

Object Object::Match(const Expression& other) const
{
	if (null())
		return boolean(false);
	return expr->Match(other);
}

Object Object::Compute(const Knowledge& knowledge) const
{
    return expr->Compute(knowledge);
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
	return reinterpret_cast<size_t>(expr.get());
}

Object Object::Cast(const std::type_info& t, const Knowledge& knowledge) const
{
    return expr->Cast(t, knowledge);
}

}
}
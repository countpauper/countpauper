#pragma once
#include "Namespace.h"
#include "Object.h"

namespace Angel
{
namespace Logic
{

// Knowledge is a tree of expressions organized in namespaces
// TODO: it should itself be an expression and namespaces as well 
// They are not exactly a conjunction, because they may also include functions 
class Knowledge
{
public:
	Knowledge();
    size_t Know(Object&& e);
	Object Query(const Object& e, const Variables& substitutions=Variables()) const;
    Object Match(const Expression& e) const;
	bool Knows(const Object& e) const;
	size_t Clauses() const;
private:
	Object Query(const Expression& e, const Variables& substitutions) const;
	Namespace root;
};


}
}
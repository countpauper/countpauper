#pragma once
#include "Namespace.h"
#include "Object.h"

namespace Angel
{
namespace Logic
{

// Knowledge is a set of exprs organized in namespaces
class Knowledge
{
public:
	Knowledge();
    size_t Know(Object&& e);
	Object Query(const Object& e) const;
    Object Query(const Expression& e) const;
    Object Match(const Expression& e) const;
	bool Knows(const Object& e) const;
	size_t Clauses() const;
private:
	Namespace root;
};


}
}
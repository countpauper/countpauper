#pragma once
//#include "Logic/Namespace.h"
#include "Logic/Object.h"
#include "Logic/Match.h"

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
	bool Knows(const Object& e) const;
	size_t size() const;
	bool empty() const;
private:
	Set root;
};


}
}
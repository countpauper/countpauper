#pragma once

#include "Logic/Match.h"
#include "Logic/Predicate.h"

namespace Angel::Logic
{
class Expression;

// Knowledge is a tree of clauses organized in namespaces
// A clause is a pair of a predicate and an expression 
// TODO: it should itself be an expression (Set?)  
class Knowledge
{
public:
	Knowledge();
    size_t Know(Predicate&& key);
    size_t Know(Predicate&& key, Expression&& value);
	Expression Matches(const Predicate& predicate) const;
	Expression Infer(const Expression& e) const;
	bool Knows(const Expression& e) const;
	const Bag& Root() const;
private:
	void AddDefaults();
	Bag root;
};


}
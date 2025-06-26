#pragma once

#include "Logic/Predicate.h"

namespace Angel::Logic
{
class Expression;
class Association;

// Knowledge is a tree of clauses organized in namespaces
// A clause is a pair of a predicate and an expression 
// TODO: it should itself be an expression (Set?)  
class Knowledge
{
public:
	Knowledge();
	explicit Knowledge(std::initializer_list<Expression> knowledge);
    size_t Know(Expression&& clause);
	size_t Forget(const Predicate& match);
	Bag Matches(const Predicate& predicate) const;
	Expression Infer(const Expression& e) const;
	Expression Knows(const Expression& e) const;
	const Bag& Root() const;
	Knowledge& Lock() const;
private:
	void AddDefaults();
	Bag root;
};


}
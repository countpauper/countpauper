#pragma once
#include "Object.h"
#include "Match.h"
#include <cstddef>

namespace Angel::Logic
{

template<class _From>
class CastException : public std::runtime_error
{
public:
    CastException(const std::type_info& to) :
        std::runtime_error((std::string("Can not Cast from ") + typeid(_From).name() + " to " + to.name()))
    {
    }
};

// An expression is a subclass of hierarchical exprs consisting of Functions (branches) or exprs (leafs)
//
class Expression
{
public:
    Expression() = default;
    virtual ~Expression() = default;
    // Try to resolve this Expression to an Elemental type in its preferred return type
    // Will return a copy for Elements compute a result for Operators 
    // for predicates  it will query the knowledge for truth and return a boolean (or unresolved conditions ?) 
    virtual Object Infer(const Knowledge& known, const Variables& substitutions) const = 0;

    // Convert an element to the given type 
    // Non-elements are infered first
    template<class _T>
    Object Cast(const Knowledge& k) const { return Cast(typeid(_T), k); }
    
    // Make a copy of the same type
    virtual Object Copy() const = 0;
    // Hash that distinguishes items of different type and value in that type
    virtual std::size_t Hash() const = 0;
    // operator bool returns true if the value is not nullish
    // this can be done without Inference
    explicit virtual operator bool() const = 0;
    // Two elements are equivalent if they are the same type *and* the same value
    // they must have the same hash  
    virtual bool operator==(const Expression& other) const = 0;
    bool operator!=(const Expression& other) const { return !operator==(other); }
    // Return a true Match, containing the requires substitutions (both ways)
    // if this expression matches the other. 
    // For elements: Match if they are equivalent
    // For variables: They always match and become a substitution
    // For collections: match is all elements match both ways. 
    // For predicates& clauses: Match if the id matches and all arguments match 
    virtual Match Matching(const Expression& other) const = 0;
    // Return whether this expresion has a condition that must be inferred 
    // if nullptr there is no condition. 
    virtual const Object* Condition() const = 0;
    // Conert the expression to a string, 
    operator std::string() const;
protected:
    friend class Object;
    virtual Object Cast(const std::type_info& t, const Knowledge& k) const = 0;
};
std::ostream& operator<<(std::ostream& os, const Angel::Logic::Expression& e);


}

namespace std
{
	template <>
	struct hash<Angel::Logic::Expression>
	{
		size_t operator()(const Angel::Logic::Expression& e) const
		{
			return typeid(e).hash_code() ^ e.Hash();
		}
	};
}


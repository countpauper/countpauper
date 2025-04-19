#pragma once
#include "Object.h"

namespace Angel
{
namespace Logic
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
    // Will return a copy for Elements and a computed result for Operators 
    // for logical queries it will query the knowledge for truth and return a boolean (or unresolved conditions ?) 
    virtual Object Compute(const Knowledge& known) const = 0;

    // Convert an element to the given type 
    // Non-elements are computed first
    template<class _T>
    Object Cast(const Knowledge& k) const { return Cast(typeid(_T), k); }
    
    // Make a copy of the same type
    virtual Object Copy() const = 0;

    // operator bool returns true if the value trivially converts to true 
    virtual operator bool() const = 0;
    // Two elements are equal if they are the same type *and* the same value 
    // Two check if two elements of different types can be cast to the same type, compare two Objects 
    // which will use Match 
    virtual bool operator==(const Expression& other) const = 0;
    bool operator!=(const Expression& other) const { return !operator==(other); }
    virtual Object Match(const Expression& other) const = 0;
    virtual std::string String() const = 0;
protected:
    friend class Object;
    virtual Object Cast(const std::type_info& t, const Knowledge& k) const = 0;
};

}
}

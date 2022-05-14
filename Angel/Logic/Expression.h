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
    
    virtual bool operator==(const Expression& other) const = 0;
    bool operator!=(const Expression& other) const { return !operator==(other); }
    virtual bool Match(const Expression& other, const Knowledge& knowledge) const;
protected:
    friend class Object;
    virtual Object Cast(const std::type_info& t, const Knowledge& k) const = 0;
};

}
}

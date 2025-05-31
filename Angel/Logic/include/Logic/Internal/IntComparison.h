#pragma once
#include "Logic/Expression.h"
#include "Logic/Internal/Comparison.h"
#include "Logic/Association.h"
#include <cassert>

namespace Angel::Logic
{

template<BinaryOperator OP> bool compare(Expression& lhs, const Expression& rhs);
template<> bool compare<BinaryOperator{L'='}>(Expression& lhs, const Expression& rhs);

template<class T> 
Expression Comparison<T>::Simplify() const
{
    // TODO: comparison of constants
    T simple = FlatCollection<T>::SimplifyItems();
    return simple;  
} 

template<typename T>
Expression Comparison<T>::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    assert(FlatCollection<T>::size()==1);  // single element equations can be matched but not inferred. Predicate argument only
    // TODO: Equation match with logical simplication
    // true & X matches true if X is true. 
    return FlatCollection<T>::front().Matches(expression, hypothesis);
}

template<typename T>
Expression Comparison<T>::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    assert(FlatCollection<T>::size()>1);   // single element equations can't be inferred only matched
    if (FlatCollection<T>::Assumptions()) {
        return Association(Boolean(true), T(FlatCollection<T>::items));
    }
    Expression first = FlatCollection<T>::front().Infer(k, hypothesis, trace);
    for(const auto& item: *this | std::views::drop(1))
    {
        auto inferred = item.Infer(k, hypothesis, trace);
        auto cast = inferred.Cast(first.AlternativeTypeId());
        // TODO: is matching the same as being equal? 
        // or do they have to be the same type or castable to the same type left or right way around
        // and how does order (not) matter then?  "23" = 23 = 23.0 those should pbly not be automatically equal 
        // but integer and float? 23.0=23   and 23=23.0 ? 
        // at least hypothesis would have to be done here. 
        if (!compare<T::ope>(first, cast))
            return Boolean(false);
    }
    return Boolean(true);
}

// TODO: Generalize with operation 
template<class T>
std::ostream& operator<<(std::ostream& os, const Comparison<T>& conjunction)
{
    bool first = true;
    for(const auto& obj: conjunction)
    {
        if (!first)
            os << T::ope;
        os << obj;
        first = false;
    }
    return os;
}

}
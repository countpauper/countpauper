#pragma once
#include "Logic/Expression.h"
#include "Logic/Internal/Compare.h"
#include "Logic/Internal/Comparison.h"
#include "Logic/Association.h"
#include <cassert>

namespace Angel::Logic
{

template<Comparator OP>
bool IsPotentiallyOrdered(const Expression& left, const Expression& right)
{
    if ((!bool(left)) || (!bool(right)))
        return true;

    if (left.IsConstant() && right.IsConstant())
        return OP(left, right);
    return true;
}

bool IsRedundantComparisson(const Expression& left, const Expression& right);

// This is a tricky algorithm. The caller iterates over the element
// 1.  the order of all non variables needs to be determined. if any of them are not ordered left to
// right then the result is false. 
// 2. Any constant that is redundant is removed these are: 
//      .1 Constants between two constants 
//      .2 Constants between an end and a constant
// 3. If all items are removed except the constant start and the constant end, then true is returned. 
template<class T> 
Expression Simplify(T&& container)
{
    if (container.size()<=1)
        return std::move(container);
    Expression lastConstant; 
    bool allConst = true;

    for(auto it = container.begin(); it!=container.end();)
    {
        if (!IsPotentiallyOrdered<T::ope>(lastConstant, *it))
            return False;
        if (it->IsConstant())
            lastConstant = *it;
        else
            allConst = false;

        bool hasPrev = it!=container.begin(); 
        bool prevRedundant= false; 
        if (hasPrev) 
        {
            auto prev = it-1;
            prevRedundant = IsRedundantComparisson(*prev, *it);    
        }
        auto next = it+1;
        bool hasNext = next!=container.end(); 
        bool nextRedundant = false;
        if (hasNext)
        {
            nextRedundant = IsRedundantComparisson(*it, *next);
        }
        bool erase = false; 
        erase |= !hasPrev && nextRedundant;
        erase |= prevRedundant && nextRedundant;
        erase |= prevRedundant && !hasNext;
        if (erase)
            container.erase(it);
        else 
            ++it;
    }
    if (allConst) {
        return True;
    } else 
        return std::move(container);
}

template<Comparator CO> 
Expression Comparison<CO>::Simplify() const
{
    auto simple = FlatTuple<Comparison<CO>>::SimplifyItems();
    return Angel::Logic::Simplify<Comparison<CO>>(std::move(simple));
} 

template<Comparator CO> 
bool Comparison<CO>::HasLeftAssumption() const
{
    if (FlatTuple<Comparison<CO>>::empty())
        return false;
    return !FlatTuple<Comparison<CO>>::front().Assumptions().empty();
}

template<Comparator CO> 
Expression Comparison<CO>::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    auto substituted = Substitute(hypothesis);
    assert(substituted.size()==1);  // single element equations can be matched but not inferred. Predicate argument only
    Hypothesis newHypothesis(hypothesis);
    Tuple::const_iterator front = substituted.begin();
    substituted.AddAt(front, expression.Substitute(hypothesis));
    newHypothesis.Add(substituted);
    return newHypothesis;
}

template<Comparator CO> 
Expression Comparison<CO>::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    assert(FlatTuple<Comparison<CO>>::size()>1);   // single element equations can't be inferred only matched
    if (!FlatTuple<Comparison<CO>>::Assumptions().empty()) {
        return Association(True, Comparison<CO>(static_cast<const Tuple&>(*this)));
    }
    Expression first = FlatTuple<Comparison<CO>>::front().Infer(k, hypothesis, trace);
    for(const auto& item: *this | std::views::drop(1))
    {
        auto inferred = item.Infer(k, hypothesis, trace);
        auto cast = inferred.Cast(first.AlternativeTypeId());
        // TODO: is matching the same as being equal? 
        // or do they have to be the same type or castable to the same type left or right way around
        // and how does order (not) matter then?  "23" = 23 = 23.0 those should pbly not be automatically equal 
        // but integer and float? 23.0=23   and 23=23.0 ? 
        // at least hypothesis would have to be done here. 
        if (!CO(first, cast))
            return False;
    }
    return True;
}

// TODO: Generalize with operation 
template<Comparator CO>
std::ostream& operator<<(std::ostream& os, const Comparison<CO> & comparison)
{
    bool first = comparison.size()>1;
    for(const auto& obj: comparison)
    {
        if (!first)
            os << CO;
        os << obj;
        first = false;
    }
    return os;
}

}
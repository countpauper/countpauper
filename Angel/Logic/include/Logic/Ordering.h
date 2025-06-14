#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"
#include <iostream>

namespace Angel::Logic
{

template<Comparator O>
class Ordering : public Comparison<Ordering<O>> 
{
public:
    using Comparison<Ordering<O>>::Comparison;
    bool operator==(const Ordering& rhs) const { return ope==rhs.ope && Comparison<Ordering<O>>::operator==(rhs); }
    constexpr static Comparator ope = O;
};

using Lesser = Ordering<Comparator{L'<'}>;
using LesserEqual = Ordering<Comparator{L'≤'}>;
using Greater = Ordering<Comparator{L'>'}>;
using GreaterEqual = Ordering<Comparator{L'≥'}>;

template < typename T >
concept is_ordering = std::derived_from<decltype(T::ope), Comparator>;

}
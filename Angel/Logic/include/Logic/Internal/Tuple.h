#pragma once
#include "Logic/Operator.h"
#include "Logic/Hypothesis.h"
#include "Logic/Operator.h"
#include <vector>
#include <initializer_list>

namespace Angel::Logic
{
using Tuple_subrange = std::ranges::subrange<std::vector<Expression>::const_iterator, std::vector<Expression>::const_iterator>;

template<typename T> class FlatTuple;

// A collection is a base ordered container of non-unique objects (wrapped in nodes)
class Tuple : public std::vector<Expression>
{
public:
    Tuple() = default;
    explicit Tuple(Tuple_subrange items);
	explicit Tuple(std::vector<Expression>&& items);
	Tuple(std::initializer_list<Expression> items);
    template<typename T> 
    Tuple(std::initializer_list<T> items)
    {
        for(auto&& item: items)
        {
            emplace_back(item);
        }
    }
    ~Tuple();
    Set Assumptions() const;
    Expression Get(const Expression& key) const;
    unsigned Add(Expression&& key);
    unsigned Remove(const Expression& key);
    Expression Matches(Tuple_subrange range, const Hypothesis& hypothesis) const;
    std::size_t Hash() const;
    bool operator==(const Tuple& rhs) const;
	Tuple& operator+=(const Tuple& rhs);
	Tuple& operator-=(const Tuple& rhs);
	Tuple& operator&=(const Tuple rhs);
	Tuple& operator|=(const Tuple rhs);
protected:
    template<typename U>
    friend class FlatTuple;
    const_iterator Find(const Expression& key) const;
    Tuple SimplifyItems() const;
    Tuple SubstituteItems(const Hypothesis& hypothesis) const;
    
};

template < typename T >
concept is_collection = std::derived_from<T, Tuple> && !is_operation<T>;

}

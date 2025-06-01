#pragma once
#include "Logic/Operator.h"
#include <vector>
#include <initializer_list>
#include <variant>
#include <algorithm>

namespace Angel::Logic
{

class Expression;
using Hypothesis = class Conjunction;
using Collection_subrange = std::ranges::subrange<std::vector<Expression>::const_iterator, std::vector<Expression>::const_iterator>;

template<typename T> class FlatCollection;

// A collection is a base ordered container of non-unique objects (wrapped in nodes)
class Collection : public std::vector<Expression>
{
public:
    Collection() = default;
    explicit Collection(Collection_subrange items);
	explicit Collection(std::vector<Expression>&& items);
	Collection(std::initializer_list<Expression> items);
    template<typename T> 
    Collection(std::initializer_list<T> items)
    {
        for(auto&& item: items)
        {
            emplace_back(item);
        }
    }
    ~Collection();
    std::size_t Assumptions() const;
    Expression Get(const Expression& key) const;
    Expression Matches(Collection_subrange range, const Hypothesis& hypothesis) const;
    std::size_t Hash() const;
    bool operator==(const Collection& rhs) const;
protected:
    template<typename U>
    friend class FlatCollection;

    Collection SimplifyItems() const;
    Collection SubstituteItems(const Hypothesis& hypothesis) const;
    
};

template < typename T >
concept IsCollection = std::derived_from<T, Collection> && !IsOperation<T>;

}

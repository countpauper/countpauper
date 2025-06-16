#pragma once
#include "Logic/Operator.h"
#include "Logic/Hypothesis.h"
#include <vector>
#include <initializer_list>
#include <variant>
#include <algorithm>

namespace Angel::Logic
{
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
    Set Assumptions() const;
    Expression Get(const Expression& key) const;
    unsigned Add(const Expression& key);
    unsigned Remove(const Expression& key);
    Expression Matches(Collection_subrange range, const Hypothesis& hypothesis) const;
    std::size_t Hash() const;
    bool operator==(const Collection& rhs) const;
	Collection& operator+=(const Collection& rhs);
	Collection& operator-=(const Collection& rhs);
	Collection& operator&=(const Collection rhs);
	Collection& operator|=(const Collection rhs);
protected:
    template<typename U>
    friend class FlatCollection;
    const_iterator Find(const Expression& key) const;
    Collection SimplifyItems() const;
    Collection SubstituteItems(const Hypothesis& hypothesis) const;
    
};

template < typename T >
concept is_collection = std::derived_from<T, Collection> && !is_operation<T>;

}

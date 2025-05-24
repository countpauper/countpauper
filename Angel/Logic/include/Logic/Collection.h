#pragma once
#include "Logic/Operator.h"
#include <vector>
#include <initializer_list>
#include <variant>
#include <algorithm>

namespace Angel::Logic
{

class Expression;
using Variables = class Conjunction;

template<typename T> class FlatCollection;

// A collection is a base ordered container of non-unique objects (wrapped in nodes)
class Collection : public std::vector<Expression>
{
public:
    Collection() = default;
	Collection(std::initializer_list<Expression> items);
	explicit Collection(std::vector<Expression>&& items);
    template<typename T> 
    Collection(std::initializer_list<T> items)
    {
        for(auto&& item: items)
        {
            emplace_back(item);
        }
    }
    ~Collection();
    Expression Get(const Expression& key) const;
    std::size_t Hash() const;
    bool operator==(const Collection& rhs) const;
protected:
    // template<typename T>
    // friend T FlatCollection<T>::SimplifyItems() const;

    // template<typename T>
    // friend T FlatCollection<T>::SubstituteItems(const Variables&) const;

    template<typename U>
    friend class FlatCollection;

    Collection SimplifyItems() const;
    Collection SubstituteItems(const Variables& substitutions) const;
    
};

template < typename T >
concept IsCollection = std::derived_from<T, Collection> && !IsOperation<T>;



}

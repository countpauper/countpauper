#pragma once
#include <vector>
#include <initializer_list>
#include "Logic/Operator.h"

namespace Angel::Logic
{

class Expression;


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
protected:
    bool operator==(const Collection& rhs) const;
};

template < typename T >
concept IsCollection = std::derived_from<T, Collection> && !IsOperation<T>;

}

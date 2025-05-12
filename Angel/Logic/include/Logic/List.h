#pragma once
#include "Logic/Match.h"
#include <vector>
#include <iostream>

namespace Angel::Logic
{

class Object;

// A list is an ordered non-unique collection of objects (wrapped in nodes)
class List : public std::vector<Object>
{
public:
    List() = default;
	List(std::initializer_list<Object> setItems);
    template<typename T> 
    explicit List(std::initializer_list<T> items)
    {
        for(auto&& item: items)
        {
            emplace_back(item);
        }
    }
    bool operator==(const List& rhs) const;
    operator bool() const;
    Match Matches(const List& other, const Variables& substitutions) const;
    std::size_t Hash() const;
};

std::ostream& operator<<(std::ostream& os, const List& list);

}

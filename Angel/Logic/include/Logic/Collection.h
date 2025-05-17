#pragma once
#include <vector>
#include <initializer_list>

namespace Angel::Logic
{

class Expression;
class Object;

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
protected:
    bool operator==(const Collection& rhs) const;
    std::size_t Hash() const;
};

}

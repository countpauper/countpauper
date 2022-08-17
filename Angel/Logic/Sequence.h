#pragma once
#include <vector>
#include "Collection.h"
#include "Object.h"
#include "AllTrue.h"

namespace Angel
{
namespace Logic
{


// A sequence is an unordered collection of Objects
// The text format is <Object>, <Object> 
class Sequence : public Collection, public std::vector<Object>
{
public:
    Sequence();
    explicit Sequence(Object&& value);
    template<class ...Args>
    explicit Sequence(Object&& first, Args... args)
    {
        Add(std::move(first));
        Merge(std::move(Sequence(std::forward<Args>(args)...)));
    }

    Sequence(const Sequence&);
    Sequence& operator=(const Sequence&) = delete;
    Sequence(Sequence&& other);
    bool operator==(const Expression& other) const override;
    std::string String() const override;
    Object Copy() const override;
    void Add(Object&& value);
    void Merge(Sequence&& other);
    Object Match(const Expression& other) const override;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
};

Object sequence();
Object sequence(Sequence&& seq);
Object sequence(Sequence&& left, Sequence&& right);
Object sequence(Sequence&& left, Object&& right);
Object sequence(Object&& left, Sequence&& right);


template<class ...Args>
Object sequence(Args... args)
{
    return Object(std::make_unique<Sequence>(std::forward<Args>(args)...));
}

}
}

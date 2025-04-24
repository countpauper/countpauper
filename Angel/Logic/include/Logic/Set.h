#pragma once
#include <unordered_set>
#include "Collection.h"
#include "Object.h"
#include "AllTrue.h"

namespace Angel
{
namespace Logic
{

class Sequence;

class Set : public Collection, public std::unordered_set<Object>
{
public:
	Set();
    explicit Set(const Set& set);
    explicit Set(Set&& set);
    explicit Set(Sequence&& set);

	template<class ...Args, class = std::enable_if_t <
		all_true < std::is_convertible<Args, Object>{}... > {}
	>>
		explicit Set(Object&& first, Args... args)
    {
        Add(std::move(first));
		Merge(Set(std::forward<Args>(args)...));
	}

	Set& operator=(const Set&) = delete;
    Object Copy() const override;
    operator bool() const override;
    bool operator==(const Expression& other) const override;
    std::size_t Hash() const override;
    Match Matching(const Expression& other) const override;
	void Add(Object&& value);
	void Merge(Set&& other);
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
	friend std::ostream& operator<<(std::ostream& os, const Set& set);	
};

std::ostream& operator<<(std::ostream& os, const Set& set);

Object set();
Object set(Sequence&& seq);
Object set(Set&& s, Object&& o);

template<class ...Args, class = std::enable_if_t <
	all_true < std::is_convertible<Args, Object>{}... > {}
>>
Object set(Args... args)
{
	return Create<Set>(std::forward<Args>(args)...);
}

}
}

#pragma once

#include "Logic/Element.h"
#include "Logic/Match.h"
#include <map>

namespace Angel::Logic
{

class Expression;

class Set
{
public:
	Set() = default;
	Set(std::initializer_list<Expression> setItems);
	template<typename T> 
	Set(std::initializer_list<T> setItems)
	{
		for(auto&& item: setItems)
		{
			Add(std::move(item));
		}
	}
	void Add(Expression&& other);

	std::size_t size() const;
	const Expression* Get(const Expression& key) const;
    Match Matches(const Expression& other, const Variables& variables) const;
    Expression Infer(const class Knowledge& knowledge, const Variables& substitutions) const;
	explicit operator bool() const;
	std::size_t Hash() const;
	bool operator==(const Set& rhs) const;
	class iterator
	{
	public:
		Expression operator*() const;
		iterator& operator++();
		iterator operator++(int);
        bool operator==(const iterator& rhs) const; 
	friend class Set;

	private:
		using Inner = std::map<Expression,Expression>; 
		iterator(Inner::const_iterator i);
		Inner::const_iterator it; 
	};
	iterator begin() const;
	iterator end() const;
    static constexpr bool unique=true;
    static constexpr bool ordered=false;

	friend std::ostream& operator<<(std::ostream& os, const Set& set);
private:
	std::map<Expression, Expression> items;

};

std::ostream& operator<<(std::ostream& os, const Set& set);

}

#pragma once

#include "Logic/Element.h"
#include "Logic/Match.h"
#include <map>

namespace Angel::Logic
{

class Expression;

class Set : public std::map<Expression, Expression>
{
public:
	Set() = default;
	Set(std::initializer_list<Expression> setItems);
	template<typename T> 
	Set(std::initializer_list<T> setItems)
	{
		for(auto&& item: setItems)
		{
			emplace(std::make_pair(std::move(item), Boolean(true)));
		}
	}
	const Expression* Get(const Expression& key) const;
    Match Matches(const Expression& other, const Variables& variables) const;
    Expression Infer(const class Knowledge& knowledge, const Variables& substitutions) const;
	explicit operator bool() const;
	std::size_t Hash() const;
	bool operator==(const Set& rhs) const;

    static constexpr bool unique=true;
    static constexpr bool ordered=false;
};

std::ostream& operator<<(std::ostream& os, const Set& set);

}

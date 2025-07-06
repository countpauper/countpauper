#pragma once

#include "Logic/Element.h"
#include "Logic/Hypothesis.h"
#include "Logic/Hash.h"
#include <unordered_map>
#include <optional>
#include <vector> 

namespace Angel::Logic
{

class Set
{
public:
	Set() = default;
	Set(std::initializer_list<Expression> setItems);
	explicit Set(std::vector<Expression>&& items);	
	
	unsigned Add(Expression&& other);
	unsigned Remove(const Expression& e);
	std::size_t size() const;
	bool empty() const;
    Expression Get(const Expression& key) const;
	Expression Pop(const Expression& e);
    Set Simplify() const;
	Set Assumptions() const;
    Expression Matches(const Expression& other, const Hypothesis& substitions) const;
    Set Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;
	explicit operator bool() const;
	std::size_t Hash() const;
	bool operator==(const Set& rhs) const;
	Set& operator+=(const Container& rhs);
	Set& operator-=(const Container& rhs);
	Set& operator&=(Container rhs);
	Set& operator|=(const Container& rhs);
	class const_iterator
	{
	public:
		Expression operator*() const;
		Expression* operator->() const;
		const_iterator& operator++();
		const_iterator operator++(int);
        bool operator==(const const_iterator& rhs) const; 
	friend class Set;

	private:
		using Inner = std::unordered_map<Expression,Expression>; 
		const_iterator(Inner::const_iterator i);
		Inner::const_iterator it; 
	};
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator erase(const_iterator item);
    static constexpr bool unique=true;
    static constexpr bool ordered=false;

	friend std::ostream& operator<<(std::ostream& os, const Set& set);
private:
	Expression MakeHypothesis(const Set& constants, bool reversed) const;
	std::unordered_map<Expression, Expression> items;

};

Set operator+(Set lhs, const Container& rhs);
Set operator-(Set lhs, const Container& rhs);
Set operator&(Set lhs, const Container& rhs);
Set operator|(Set lhs, const Container& rhs);
std::ostream& operator<<(std::ostream& os, const Set& set);


}

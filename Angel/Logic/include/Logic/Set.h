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
	Set& operator+=(const Set& rhs);
	Set& operator-=(const Set& rhs);
	Set& operator&=(const Set& rhs);
	Set& operator|=(const Set& rhs);
	class iterator
	{
	public:
		Expression operator*() const;
		iterator& operator++();
		iterator operator++(int);
        bool operator==(const iterator& rhs) const; 
	friend class Set;

	private:
		using Inner = std::unordered_map<Expression,Expression>; 
		iterator(Inner::const_iterator i);
		Inner::const_iterator it; 
	};
	iterator begin() const;
	iterator end() const;
    static constexpr bool unique=true;
    static constexpr bool ordered=false;

	friend std::ostream& operator<<(std::ostream& os, const Set& set);
private:
	Expression MakeHypothesis(const Set& constants, bool reversed) const;
	std::unordered_map<Expression, Expression> items;

};

Set operator+(Set lhs, const Set& rhs);
Set operator-(Set lhs, const Set& rhs);
Set operator&(Set lhs, const Set& rhs);
Set operator|(Set lhs, const Set& rhs);
std::ostream& operator<<(std::ostream& os, const Set& set);

}

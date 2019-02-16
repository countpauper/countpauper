#include "stdafx.h"
#include "Sequence.h"

namespace Angel
{
namespace Logic
{

Sequence::Sequence() 
{
}

Sequence::Sequence(Element&& value)
{
	contents.emplace_back(std::move(value));
}


/* 	// Elements have to be copyable or no initializer list ... 
Sequence::Sequence(const std::initializer_list<Element&&>& init) 
{
	for (auto it = init.begin(); it!=init.end(); ++it)
	{
		contents.emplace_back(std::move(*it));
	}
}
*/

Sequence::Sequence(Sequence&& other) :
	contents(std::move(other.contents))
{
}


bool Sequence::operator==(const Value& value) const
{
	if (auto sequence= dynamic_cast<const Sequence*>(&value))
	{
		return true;	// TODO: check all elements
	}
	return false;
}

size_t Sequence::size() const
{
	return contents.size();
}

void Sequence::Append(Element&& value)
{
	contents.emplace_back(std::move(value));
}

void Sequence::Merge(Sequence&& other)
{
	for (auto& e : other.contents)
		contents.emplace_back(std::move(e));
}


Element sequence()
{
	return Element(std::make_unique<Sequence>());
}
/*
Element sequence(const std::initializer_list<Element&&>& init)
{
	return Element(std::make_unique<Sequence>(init));
}
*/


}
}
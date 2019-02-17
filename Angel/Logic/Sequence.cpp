#include "stdafx.h"
#include "Sequence.h"
#include "Array.h"

namespace Angel
{
namespace Logic
{

Sequence::Sequence() 
{
}

Sequence::Sequence(Array&& array)
{
	for (auto& e : array)
	{
		emplace_back(std::move(e));
	}
}

Sequence::Sequence(Element&& value)
{
	emplace_back(std::move(value));
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
	std::vector<Element>(std::move(other))
{
}


bool Sequence::operator==(const Value& value) const
{
	if (auto sequence= dynamic_cast<const Sequence*>(&value))
	{
		if (size() != sequence->size())
			return false;
		auto valueIt = sequence->begin();
		for (auto it = begin(); it!=end(); ++it, ++valueIt)
		{
			if (it->operator!=(*valueIt))
				return false;
		}
		return true;	
	}
	return false;
}

bool Sequence::Match(const Value& value, const Knowledge& knowledge) const
{
	if (auto sequence = dynamic_cast<const Sequence*>(&value))
	{
		if (size() != sequence->size())
			return false;
		auto it = sequence->begin();
		for (const auto& e : *this)
		{
			if (!e.Match(*it, knowledge))
				return false;
			++it;	// TODO: zip
		}
		return true;
	}
	return false;
}


void Sequence::Append(Element&& value)
{
	emplace_back(std::move(value));
}

void Sequence::Merge(Sequence&& other)
{
	for (auto& e : other)
		emplace_back(std::move(e));
}


Element sequence()
{
	return Element(std::make_unique<Sequence>());
}


Element sequence(Array&& array)
{
	return Element(std::make_unique<Sequence>(std::move(array)));
}

/*
Element sequence(const std::initializer_list<Element&&>& init)
{
	return Element(std::make_unique<Sequence>(init));
}
*/


}
}
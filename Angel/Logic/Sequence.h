#pragma once
#include <vector>
#include "Id.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

class Sequence : public Value
{
public:
	Sequence();
	Sequence(Element&& value);
	template<class ...Args>
	Sequence(Element&& first, Args... args) :
		Sequence(std::move(first))
	{
		Merge(Sequence(std::forward<Args>(args)...));
	}

//	Sequence(const std::initializer_list<Element&&>& init);
	Sequence(const Sequence&) = delete;
	Sequence& operator=(const Sequence&) = delete;
	Sequence(Sequence&& other);

	bool operator==(const Value& other) const override;
	size_t size() const;
	void Append(Element&& value);
	void Merge(Sequence&& other);
private:
	std::vector<Element> contents;
};

Element sequence();

template<class ...Args>
Element sequence(Args... args)
{
	return Element(std::make_unique<Sequence>(std::forward<Args>(args)...));
}

}
}

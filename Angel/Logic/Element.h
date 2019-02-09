#pragma once
#include <memory>

namespace Angel
{
namespace Logic
{

class Value
{
public:
	Value() = default;
	virtual ~Value() = default;
	virtual bool operator==(const Value& other) const = 0;
	virtual bool Match(const Value& other) const;
};

class Element final
{
public:
	Element() = default;

	Element(std::unique_ptr<Value>&& v) :
		value(std::move(v))
	{
	}
	
	Element(const Element& other) = delete;
	Element& operator=(const Element& other) = delete;
	Element(Element&& other);
	Element& operator=(Element&& other);

	bool operator==(const Element& other) const;
	bool Match(const Element& other) const;
private:
	std::unique_ptr<Value> value;
};

template<class T, typename... Args>
Element Create(Args... args)
{
	return Element(std::make_unique<T>(std::forward<Args>(args)...));
}


}
}
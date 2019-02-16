#pragma once
#include <memory>

namespace Angel
{
namespace Logic
{

class Knowledge;

class Value
{
public:
	Value() = default;
	virtual ~Value() = default;
	virtual bool operator==(const Value& other) const = 0;
	bool operator!=(const Value& other) const { return !operator==(other); }
	virtual bool Match(const Value& other, const Knowledge& knowledge) const;
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

	operator bool() const;
	bool operator==(const Element& other) const;
	bool operator!=(const Element& other) const { return !operator==(other); }
	bool Match(const Element& other, const Knowledge& knowledge) const;

	template<class C>
	C* Cast() const { return dynamic_cast<C*>(value.get()); }
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
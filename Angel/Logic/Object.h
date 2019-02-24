#pragma once
#include <memory>
#include <string>

namespace Angel
{
namespace Logic
{

class Knowledge;
class Item;

class Object final
{
public:
	Object() = default;

	Object(std::unique_ptr<Item>&& v) :
		item(std::move(v))
	{
	}
	Object(const std::wstring& tag);
	Object(const Object& other) = delete;
	Object& operator=(const Object& other) = delete;
	Object(Object&& other);
	Object& operator=(Object&& other);

	operator bool() const;
	bool Trivial() const;
	bool operator==(const Object& other) const;
	bool operator!=(const Object& other) const { return !operator==(other); }
	bool Match(const Object& other, const Knowledge& knowledge) const;

	template<class C>
	C* Cast() const { return dynamic_cast<C*>(item.get()); }
	size_t Hash() const;
private:
	std::unique_ptr<Item> item;
};

template<class T, typename... Args>
Object Create(Args... args)
{
	return Object(std::make_unique<T>(std::forward<Args>(args)...));
}
}
}

namespace std
{
	template <>
	struct hash<Angel::Logic::Object>
	{
		size_t operator()(const Angel::Logic::Object& e) const
		{
			return e.Hash();
		}
	};
}

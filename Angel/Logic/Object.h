#pragma once

#include <memory>
#include <string>

namespace Angel
{
namespace Logic
{

class Knowledge;
class Expression;

// An object is a variadic instance of any type of expr
class Object final
{
public:
    Object() = default;
    explicit Object(std::unique_ptr<Expression>&& v) :
        expr(std::move(v))
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
	bool Match(const Expression& other, const Knowledge& knowledge) const;
    const Expression& operator*() const;
    Object Compute(const Knowledge& knowledge) const;
	template<class C>
    C* As() const { return dynamic_cast<C*>(expr.get()); }
    template<class C>
    Object Cast(const Knowledge& knowledge) const { return Cast(typeid(C), knowledge); } 
    size_t Hash() const;
private:
    friend class Expression;
    Object Cast(const std::type_info& t, const Knowledge& knowledge) const;
	std::unique_ptr<Expression> expr;
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

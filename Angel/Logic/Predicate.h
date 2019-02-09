#pragma once
#include "Element.h"
#include "Id.h"

namespace Angel
{
namespace Logic
{

class Predicate : public Value
{
public:
	explicit Predicate(const Id& id);
	explicit Predicate(const std::wstring& name);
	bool operator==(const Value& other) const override;
private:
    Id id;
};

}
}
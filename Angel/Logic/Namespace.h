#pragma once
#include <unordered_set>
#include "Id.h"
#include "Object.h"

namespace Angel
{
namespace Logic
{

class Namespace
{
public:
	explicit Namespace(const Id& id);
	void Add(Object&& e);
	bool Match(const Object& e, const Knowledge& knowledge) const;
	bool Contains(const Object& e) const;
	size_t Clauses() const;
private:
    Id id;
	std::unordered_set<Object> contents;
};


}
}
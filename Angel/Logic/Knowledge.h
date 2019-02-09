#pragma once
#include "Namespace.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

class Knowledge
{
public:
	Knowledge();
	void Know(Element&& e);
	bool Query(const Element& e) const;
private:
	Namespace root;
};


}
}
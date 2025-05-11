#include "Logic/Node.h"
#include "Logic/Object.h"

namespace Angel::Logic
{

bool Node::operator<(const Node&o) const
{
    std::hash<Node> hasher; 
    return hasher(*this) < hasher(o);
}

bool Node::operator==(const Node& o) const
{
    return value == o.value;
}

}

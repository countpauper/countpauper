#pragma once
#include "Item.h"

namespace Angel
{
namespace Logic
{

// An element is a sub category of items that is not a compound of other items
// For instance an Integer, Boolean is an Item
class Element : public Item
{
public:
};

}
}

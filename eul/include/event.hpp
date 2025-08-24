#pragma once 

#include "identifier.hpp"

namespace eul
{

class event 
{
public:
    bool operator==(const event& o) const;
private:
    identifier<> _id;
};

}

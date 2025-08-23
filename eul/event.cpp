#include "event.hpp"

namespace eul 
{
    

event::event() 
{
    _last_Id++;
    _id = _last_Id;
}

event::event(const event& o) :
    _id(o._id)
{
}
event& event::operator=(const event& o)
{
    _id = o._id;
    return *this;
}

bool event::operator==(const event& o) const
{
    return _id == o._id;
}

unsigned event::_last_Id = 0;

}

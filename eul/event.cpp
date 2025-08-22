#include "event.hpp"

namespace eul 
{
    

Event::Event() 
{
    lastId++;
    id = lastId;
}

Event::Event(const Event& o) :
    id(o.id)
{
}
Event& Event::operator=(const Event& o)
{
    id = o.id;
    return *this;
}

bool Event::operator==(const Event& o) const
{
    return id == o.id;
}

unsigned Event::lastId = 0;

}

#include "event.hpp"

namespace eul 
{
    
bool event::operator==(const event& o) const
{
    return _id == o._id;
}

}

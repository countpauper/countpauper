#pragma once 

namespace eul
{

template<typename ID=unsigned>
class identifier 
{
public:
    identifier() :
        _id(new_id())
    {
    }

    identifier(const identifier& o) :
       _id(o._id)
    {
    }
    
    identifier(identifier&& o) :
       _id(o._id)
    {
        o._id = 0;
    }
    
    identifier& operator=(const identifier& o)
    {
        _id == o._id;
        return *this;
    }

    identifier& operator=(identifier&& o)
    {
        _id = o._id;
        o._id = 0;
        return *this;
    }

    operator bool() const 
    {
        return _id!=0;
    }

    bool operator==(const identifier& o) const
    {
        return _id == o._id;
    }
private:
    ID new_id() const
    {
        return ++_last_id;
    }
    ID _id;
    static ID _last_id;
};

// TODO: this shares the last id beteen all ids of the same ID type, 
// not with the same member in a parent class. This will run of IDs fast if 
// ID<uint8_t> is used in more than one place.
// Test this and fix this (curiously recurring template pattern?)
template<typename ID> 
ID identifier<ID>::_last_id = 0; 

}

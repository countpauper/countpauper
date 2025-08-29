#include "copy_counter.hpp"
#include <utility>

namespace eul 
{

int copy_count::current() const
{
    return (constructed + copied) - (destructed + assigned);
}

void copy_count::reset()
{
    constructed = 0;
    copied = 0;
    moved = 0;
    assigned = 0;
    destructed = 0;
}

copy_counter::copy_counter(copy_count& count) 
    : _count(&count)
{
    ++_count->constructed;
}

copy_counter::~copy_counter()
{
    if (_count)
        ++_count->destructed;
}
copy_counter::copy_counter(const copy_counter& original) : 
    _count(original._count)
{
    if (_count)
        ++_count->copied;
}

copy_counter::copy_counter(copy_counter&& original) :
    _count(original._count)
{
    original._count = nullptr;
    if (_count)
        ++_count->moved;
}

copy_counter& copy_counter::operator=(const copy_counter& original)
{
    if (_count)
        ++_count->assigned;
    _count = original._count;
    if (_count)
        ++_count->copied;

    return *this;
}

copy_counter& copy_counter::operator=(copy_counter&& original)
{
    if (_count)
        ++_count->assigned;
    _count = nullptr;
    
    std::swap(_count, original._count);
    if (_count)
        ++_count->moved;

    return *this;
}

bool copy_counter::operator==(const copy_counter& other) const
{
    return _count!=nullptr && _count == other._count;
}

copy_counter::operator bool() const
{
    return _count!=nullptr;
}

const copy_count* copy_counter::operator->() const
{
    return _count;
}

copy_count copy_counter::default_count;

}
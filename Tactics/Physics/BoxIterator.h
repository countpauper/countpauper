#pragma once
#include "Box.h"

namespace Physics
{
template<class _D, typename _V>
class BoxIterator
{
public:
    // iterate over all data
    explicit BoxIterator(_D& data) : BoxIterator(data, data.Insides()) {}
    BoxIterator(_D& data, const Box& box) : BoxIterator(data, box, box.Start()) {}
    BoxIterator(_D& data, const Box& box, const Position& position) : data(data), box(box), position(position)
    {
        // clip to end
        if (!box.Contains(position))
        {
            this->position = box.End();
        }
    }
    BoxIterator<_D, _V>& operator++()
    {
        if (++position.z >= box.z.end)
        {
            if (++position.y >= box.y.end)
            {
                if (++position.x >= box.x.end)
                {
                    // end
                }
                else
                {
                    position.y = box.y.begin;
                    position.z = box.z.begin;
                }
            }
            else
            {
                position.z = box.z.begin;
            }
        }
        return *this;
    }
    //iterator operator++(int);
    bool operator==(const BoxIterator<_D, _V>& other) const
    {
        return &data == &other.data && position == other.position;
    }
    bool operator!=(const BoxIterator<_D, _V>& other) const
    {
        return &data != &other.data || position != other.position;
    }

    using value_type = std::pair<Position, _V&>;
    value_type operator*() const
    {
        return std::pair<Position, _V&>(position, data[position]);
    }
    _V* operator->() const
    {
        static_assert(false);   // this is weird, opterator -> not giving the pair of position, but can't return pointer to anything else
    }

    BoxIterator<_D, _V> end() const
    {
        return BoxIterator(data, box, box.End());
    }

    explicit operator bool() const
    {
        return box.Contains(position);
    }
    // iterator traits
    using difference_type = Position;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;
    _D& data;
    const Box box;
    Position position;
};

}
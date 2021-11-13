#pragma once
#include "Box.h"

namespace Physics
{

class BoxIterator
{
public:
    // iterate over all data
    BoxIterator(const Box& box) : BoxIterator(box, box.Start()) {}
    BoxIterator(const Box& box, const Position& position) :  box(box), position(position)
    {
        // clip to end
        if (!box.Contains(position))
        {
            this->position = box.End();
        }
    }
    BoxIterator operator++()
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
    bool operator==(const BoxIterator& other) const
    {
        return position == other.position;
    }
    bool operator!=(const BoxIterator& other) const
    {
        return position != other.position;
    }

    using value_type = Position;
    value_type operator*() const
    {
        return position;
    }
    const Position* operator->() const
    {
        return &position;
    }

    BoxIterator end() const
    {
        return BoxIterator(box, box.End());
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
    const Box box;
    Position position;
};

template<class _D, typename _V>
class BoxDataIterator 
{
public:
    // iterate over all data
    explicit BoxDataIterator(_D& data) : BoxDataIterator(data, data.Insides()) {}
    BoxDataIterator(_D& data, const Box& box) : BoxDataIterator(data, box, box.Start()) {}
    BoxDataIterator(_D& data, const Box& box, const Position& position) : data(data), box(box), position(position)
    {
        // clip to end
        if (!box.Contains(position))
        {
            this->position = box.End();
        }
    }
    BoxDataIterator<_D, _V>& operator++()
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
    bool operator==(const BoxDataIterator<_D, _V>& other) const
    {
        return &data == &other.data && position == other.position;
    }
    bool operator!=(const BoxDataIterator<_D, _V>& other) const
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

    BoxDataIterator<_D, _V> end() const
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
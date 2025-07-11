#pragma once 
#include <deque>
#include <ranges>
#include <ios>

namespace Interpreter 
{

template<typename _S>
concept StreamWithEof = requires(const _S& s) {
    { s.eof() } -> std::convertible_to<bool>;
};

template <typename S, typename T>
concept StreamExtractable = requires(S& s, T& value) {
    { s >> value } -> std::same_as<S&>;
};

template <typename S, typename T>
concept StreamExtractableWithEof =
StreamWithEof<S> && StreamExtractable<S, T>;

template<typename T, typename S>
class FifoRange
{
    S& stream;
public:
    explicit FifoRange(S& stream) requires StreamExtractableWithEof<S,T> : 
        stream(stream) 
    {}

    class Sentinel;
    class Iterator
    {
        friend Sentinel;
        S* stream;
        T value;
    public:

        using difference_type = std::ptrdiff_t;
        using value_type = T;

        explicit Iterator(S& s) : 
            stream(&s)
        {
            *stream >> value;
        }
        // Because the fifo can only be iterated forward once, the 
        // iterator can not be copied. The front of the fifo is what the iterator is pointing at
        Iterator(const Iterator&) = delete;
        Iterator& operator=(const Iterator&) = delete;
        Iterator(Iterator&& o)
        {
            std::swap(value, o.value);
            std::swap(stream, o.stream);
        }
        Iterator& operator=(Iterator&& o)
        {
            stream = nullptr;
            value = T();
            std::swap(value, o.value);
            std::swap(stream, o.stream);
            return *this;
        }
        const T& operator*() const
        {
            return value;
        }

        Iterator& operator++()
        {
            if (stream)
            (*stream) >> value;
            return *this;
        }
    
        Iterator& operator++(int) 
        { 
            auto tmp(*this);
            ++*this;  
            return tmp; 
        }
        bool operator==(const Iterator& other) const 
        { 
            return (stream == other.stream); 
        }
        bool operator!=(const Iterator& other) const { return !(*this == other); }
    };

    class Sentinel
    {
        friend class Iterator;
        S* stream = nullptr;
    public:
        Sentinel() = default;
        explicit Sentinel(S& s) : 
            stream(&s)
        {
        }
        Sentinel(const Sentinel& o) : stream(o.stream) {} 
        Sentinel& opererator(const Sentinel& o) { stream = o.stream; return *this; }
        bool operator==(const Sentinel& other) const 
        { 
            return stream == other.stream;
        }

        bool operator==(const Iterator& it) const 
        { 
            if (stream != it.stream)
                return false;
            else
                return it.stream->eof(); 
        }

        bool operator!=(const Sentinel& other) const { return !(*this == other); }
    };

    Iterator begin() { return Iterator(stream); }
    Sentinel end() { return Sentinel(stream); }
};

 
// Usage with ranges:
// A FIFO for a specific type 
template<class T>   // T is the object type, which must be copyable
class Fifo : public std::ios_base
{
public:
    Fifo() = default;
    Fifo(std::initializer_list<T> objects)
    {
        for(const auto& object: objects)
            (*this) << object;
    }

    Fifo<T>& operator<<(const T& object)
    {
        objects.push_back(object);
        return *this;
    }
    
    Fifo<T>& operator>>(T& object)
    {
        if (objects.empty())
            setstate(eofbit);
        else 
        {
            object = objects[0];
            objects.pop_front();
        }
        return *this;
    }

    const T& peek()
    {
        return objects.front();
    }


    using Range = FifoRange<T, Fifo<T>>; 
    Range View() 
    {
        return Range(*this);
    } 
    std::deque<T> Dump()
    {
        return std::move(objects);
    }

    bool eof() const 
    { 
        return state & eofbit; 
    }

    std::size_t size() const
    {
        return objects.size();
    }

    bool empty() const 
    {
        return objects.empty();
    }
    
    void setstate(iostate flags)
    {
        state |= flags;
    }
private:
    std::deque<T> objects;
    iostate state = goodbit;
};

static_assert(std::input_iterator<FifoRange<int, Fifo<int>>::Iterator>);
static_assert(std::ranges::input_range<FifoRange<int, Fifo<int>>>);
static_assert(StreamExtractableWithEof<Fifo<int>, int>);

}

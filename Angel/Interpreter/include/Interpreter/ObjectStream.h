#pragma once 
#include <deque>
#include <ranges>
#include <ios>

namespace Interpreter 
{
template <typename T>
concept StreamWithEof = requires(const T& t) {
    { t.eof() } -> std::same_as<bool>;
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
  explicit FifoRange(S& stream) : stream(stream) {}

  class Iterator
  {
      T value;
      S* stream = nullptr;
  public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
  
      Iterator() = default;
      explicit Iterator(S& stream) : 
        stream(&stream)
      {
        stream >> value;
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
        if (stream == other.stream) 
          return true;
        else if (!stream)
          return other.stream->eof();
        else if (!other.stream)
          return stream->eof();
        else 
          return false;
      }
      bool operator!=(const Iterator& other) const { return !(*this == other); }
  };
  Iterator begin() const { return Iterator(stream); }
  Iterator end() const { return Iterator(); }
};

 
// Usage with ranges:
// A FIFO for a specific type 
template<class T>   // T is the object type, which must be copyable
class Fifo : public std::ios_base
{
public:
    bool eof() const 
    { 
      return state & eofbit; 
    }

    void setstate(iostate flags)
    {
      state |= flags;
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

    using Range = FifoRange<T, Fifo<T>>; 
    Range View() 
    {
      return Range(*this);
    } 
    std::deque<T> Dump()
    {
        return std::move(objects);
    }
private:
    std::deque<T> objects;
    iostate state = goodbit;
};

static_assert(std::input_iterator<FifoRange<int, Fifo<int>>::Iterator>);
static_assert(std::ranges::input_range<FifoRange<int, Fifo<int>>>);
static_assert(StreamExtractableWithEof<Fifo<int>, int>);

}

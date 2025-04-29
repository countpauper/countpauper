#pragma once 
#include <deque>
#include <ranges>

namespace Interpreter 
{


  // TODO: tokens and symbols should be hashed. [tokens done]
  // The lexicon should be a map of hash to the term (Token or symbol). Ensure they are unique. 
  // xor  the type in there as well to distinguish symbol from token or similar regex. 
  // Terms (including symbols) should just be polymorphic then and all added to the lexicon 
  // with that the token and symbol stream would be a hash + source stream [token stream done] 
  // that can go to a char/bytes stream and that stream can be a view 
  // this also avoids dangling references. The hashes are "references" to the dictionary 
  // that can be persisted as they aren't runtime pointer

/*
    template<typename _Val, typename _Traits>
      concept __stream_extractable
	= requires(basic_istream<_Val, _Traits>& is, _Val& t) { is >> t; };
  } 

  template<movable _Val, typename _CharT,
	   typename _Traits = char_traits<_CharT>>
    requires default_initializable<_Val>
      && __detail::__stream_extractable<_Val, _CharT, _Traits>
    class basic_istream_view
    : public view_interface<basic_istream_view<_Val, _CharT, _Traits>>
    {
    public:
      constexpr explicit
      basic_istream_view(basic_istream<_CharT, _Traits>& __stream)
	: _M_stream(std::__addressof(__stream))
      { }

      constexpr auto
      begin()
      {
	*_M_stream >> _M_object;
	return _Iterator{this};
      }

      constexpr default_sentinel_t
      end() const noexcept
      { return default_sentinel; }

    private:
      basic_istream<_CharT, _Traits>* _M_stream;
      _Val _M_object = _Val();

      struct _Iterator
      {
      public:
	using iterator_concept = input_iterator_tag;
	using difference_type = ptrdiff_t;
	using value_type = _Val;

	constexpr explicit
	_Iterator(basic_istream_view* __parent) noexcept
	  : _M_parent(__parent)
	{ }

	_Iterator(const _Iterator&) = delete;
	_Iterator(_Iterator&&) = default;
	_Iterator& operator=(const _Iterator&) = delete;
	_Iterator& operator=(_Iterator&&) = default;

	_Iterator&
	operator++()
	{
	  *_M_parent->_M_stream >> _M_parent->_M_object;
	  return *this;
	}

	void
	operator++(int)
	{ ++*this; }

	_Val&
	operator*() const
	{ return _M_parent->_M_object; }

	friend bool
	operator==(const _Iterator& __x, default_sentinel_t)
	{ return __x._M_at_end(); }

      private:
	basic_istream_view* _M_parent;

	bool
	_M_at_end() const
	{ return !*_M_parent->_M_stream; }
      };

      friend _Iterator;
    };


template<class T, class A>
class VectorView : public std::ranges::view_interface<VectorView<T, A>>
{
public:
    VectorView() = default;
    
    VectorView(const std::vector<T, A>& vec) :
        m_begin(vec.cbegin()), m_end(vec.cend())
    {}
    
    auto begin() const { return m_begin; }
    
    auto end() const { return m_end; }
    
private:
    typename std::vector<T, A>::const_iterator m_begin{}, m_end{};
};
*/

// An infinite object stream uses all available memory as a buffer
// as opposed to a cirular stream
template<class T>   // T is the object type, which must be copyable
class InfiniteObjectStream : public std::basic_istream<T>
{
public:
    InfiniteObjectStream<T>& operator<<(const T& object)
    {
        objects.push_back(object);
        return *this;
    }
    InfiniteObjectStream<T>& operator>>(T& object)
    {
        object = object[0];
        objects.pop_front();
        return *this;
    }

    std::deque<T> Flush()
    {
        std::deque<T> result;
        std::swap(result, objects);
        return result;
    }    
private:
    std::deque<T> objects;
};
        

}
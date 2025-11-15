#include "intrusive_list.hpp"
#include <iterator>
#include <limits>

namespace eul 
{
intrusive_list::node::node() :
    _root(nullptr),
    _next(nullptr)
{
}

intrusive_list::node::node(class intrusive_list& _root) :
    node()
{
    _root.push_front(*this);
}

intrusive_list::node::node(const intrusive_list::node& other) :
    node()
{
    if (other._root) 
    {
        other._root->insert(other, *this);
    }
}

intrusive_list::node::node(intrusive_list::node&& other) :
    node()
{
    if (other._root) 
    {
        other._root->insert(other, *this);
        other._root->erase(other);
    }
}

intrusive_list::node& intrusive_list::node::operator=(const intrusive_list::node& other) 
{
    if (_root!=other._root) 
    {
        if (other._root)
            other._root->insert(other, *this);
        else if (_root)
            _root->erase(*this); 
    }
    return *this;
}

intrusive_list::node& intrusive_list::node::operator=(intrusive_list::node&& other)
{
    if (_root!=other._root) 
    {
        if (other._root)
            other._root->insert(other, *this);
        else if (_root)
            _root->erase(*this);

    }
    if (other._root)
        other._root->erase(other);
    return *this;
}

bool intrusive_list::node::linked() const 
{
    return _root!=nullptr;
}


void intrusive_list::node::unlink()
{
    if (_root)
        _root->erase(*this);
}

intrusive_list::node::~node() 
{
    unlink();
}

bool intrusive_list::node::operator==(const intrusive_list::node& other) const
{
    return this == &other;
}

intrusive_list::iterator::iterator()
    : _node(nullptr)
    , _ref(&_node)
{
}

intrusive_list::iterator::iterator(const pointer& n) 
    : _node(n)
    , _ref(&n)
{
}

bool intrusive_list::iterator::operator==(const intrusive_list::iterator& o) const
{
    return _node == o._node;
}

intrusive_list::iterator& intrusive_list::iterator::operator++()
{
    if (_node) {
        if (*_ref == _node ) {
            _ref = &(_node->_next);
            _node = _node->_next;
        } else {
            // The previous->next got overwritten, indicating that the current was erased (or moved)
            //  iterate to the node that repaced it 
            _node = *_ref;
        }
    }
    return *this;
}

intrusive_list::iterator intrusive_list::iterator::operator++(int)
{
    iterator prev(_node);
    ++(*this);
    return prev;
}

intrusive_list::iterator::difference_type intrusive_list::iterator::operator-(const intrusive_list::iterator& o) const
{
    difference_type distance = 0; 
    for (auto it=o; it!=iterator(); ++it, ++distance)
        if (it==*this)
            return distance;    
    if (!_node)  // this is the end() 
        return distance;
    distance = 0; 
    for(auto it=*this; it!=iterator(); ++it, --distance) 
        if (it==o)
            return distance;


    return std::numeric_limits<difference_type>::min();
}

intrusive_list::iterator::value_type& intrusive_list::iterator::operator*() const
{
    return *_node;
}

intrusive_list::iterator::value_type* intrusive_list::iterator::operator->() const
{
    return _node;
}

intrusive_list::intrusive_list() :
    _first(nullptr)
{
}

intrusive_list::iterator intrusive_list::push_front(node& n)
{
    if (n._root)
        n._root->erase(n);
    n._root = this;
    n._next = _first; 
    _first = &n;
    return iterator(_first);
}

intrusive_list::iterator intrusive_list::push_back(intrusive_list::node& node)
{
    for(auto& n: *this) 
    {
        if (!n._next)
            return insert(n, node);
    }
    return push_front(node);
}

const intrusive_list::iterator intrusive_list::begin() const
{
    return iterator(_first); 
}

const intrusive_list::iterator intrusive_list::end() const 
{
    return iterator();
}


intrusive_list::iterator intrusive_list::begin() 
{
    return iterator(_first);
}

intrusive_list::node& intrusive_list::front()
{
    return *_first;
}

intrusive_list::iterator intrusive_list::find(const node& node) const
{
    iterator it = begin();
    for(;it!=end();++it) {
        if (*it == node) 
            break;
    }
    return it;
}

intrusive_list::iterator intrusive_list::end() 
{
    return iterator(); 
}

intrusive_list::iterator intrusive_list::erase(intrusive_list::node& erased)
{
    if (_first == &erased) 
    {
        _first = erased._next;
        erased._root = nullptr;
        erased._next = nullptr;
        return begin();
    }
    for(node& n: *this)
    {
        if (n._next == &erased) 
        {
            n._next = erased._next;
            erased._next = nullptr;
            erased._root = nullptr;
            return iterator(n._next);
        }
    }
    return end();
}

void intrusive_list::clear()
{
    node* prev = nullptr;
    for(node& n: *this)
    {
        n.unlink();
    }
    _first = nullptr;
}


intrusive_list::iterator intrusive_list::insert(const node& after, node& n)
{
    if (n._root)
        n._root->erase(n);
    n._next = after._next; 
    n._root = this;
    const_cast<node&>(after)._next = &n;
    return iterator(&n);    
}

intrusive_list::iterator intrusive_list::insert(const iterator& pos, node& node)
{
    if (pos==end())
        return push_back(node);
    return insert(*pos, node);
}

std::size_t intrusive_list::size() const 
{
    return end()-begin();
}

bool intrusive_list::empty() const 
{
    return _first==nullptr;
}

}
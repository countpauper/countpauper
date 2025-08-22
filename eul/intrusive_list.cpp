#include "intrusive_list.hpp"
#include <iterator>
#include <limits>

namespace eul 
{
LinkedList::Node::Node() :
    root(nullptr),
    next(nullptr)
{
}

LinkedList::Node::Node(class LinkedList& root) :
    Node()
{
    root.push_front(*this);
}

LinkedList::Node::Node(const LinkedList::Node& other) :
    Node()
{
    if (other.root) 
    {
        other.root->insert(other, *this);
    }
}

LinkedList::Node::Node(LinkedList::Node&& other) :
    Node()
{
    if (other.root) 
    {
        other.root->insert(other, *this);
        other.root->erase(other);
    }
}

LinkedList::Node& LinkedList::Node::operator=(const LinkedList::Node& other) 
{
    if (root!=other.root) 
    {
        if (other.root)
            other.root->insert(other, *this);
        else if (root)
            root->erase(*this); 
    }
    return *this;
}

LinkedList::Node& LinkedList::Node::operator=(LinkedList::Node&& other)
{
    if (root!=other.root) 
    {
        if (other.root)
            other.root->insert(other, *this);
        else if (root)
            root->erase(*this);

    }
    if (other.root)
        other.root->erase(other);
    return *this;
}

bool LinkedList::Node::IsLinked() const 
{
    return root!=nullptr;
}


void LinkedList::Node::Unlink()
{
    if (root)
        root->erase(*this);
}

LinkedList::Node::~Node() 
{
    Unlink();
}


LinkedList::iterator::iterator() :
    node(nullptr)
{
}

LinkedList::iterator::iterator(value_type* node) : 
    node(node)
{
}

bool LinkedList::iterator::operator==(const LinkedList::iterator& o) const
{
    return node == o.node;
}

LinkedList::iterator& LinkedList::iterator::operator++()
{
    if (node)
        node = node->next;
    return *this;
}

LinkedList::iterator LinkedList::iterator::operator++(int)
{
    iterator prev(node);
    ++(*this);
    return prev;
}

LinkedList::iterator::difference_type LinkedList::iterator::operator-(const LinkedList::iterator& o) const
{
    difference_type distance = 0; 
    for (auto it=o; it!=iterator(); ++it, ++distance)
        if (it==*this)
            return distance;    
    if (!node)  // this is the end() 
        return distance;
    distance = 0; 
    for(auto it=*this; it!=iterator(); ++it, --distance) 
        if (it==o)
            return distance;


    return std::numeric_limits<difference_type>::min();
}

LinkedList::iterator::value_type& LinkedList::iterator::operator*() const
{
    return *node;
}

LinkedList::iterator::value_type* LinkedList::iterator::operator->() const
{
    return node;
}

LinkedList::LinkedList() :
    first(nullptr)
{
}

LinkedList::iterator LinkedList::push_front(Node& node)
{
    if (node.root)
        node.root->erase(node);
    node.root = this;
    node.next = first; 
    first = &node;
    return iterator(first);
}

LinkedList::iterator LinkedList::push_back(LinkedList::Node& node)
{
    for(auto& n: *this) 
    {
        if (!n.next)
            return insert(n, node);
    }
    return push_front(node);
}

const LinkedList::iterator LinkedList::begin() const
{
    return iterator(first); 
}

const LinkedList::iterator LinkedList::end() const 
{
    return iterator();
}


LinkedList::iterator LinkedList::begin() 
{
    return iterator(first);
}

LinkedList::Node& LinkedList::front()
{
    return *first;
}


LinkedList::iterator LinkedList::end() 
{
    return iterator(); 
}

LinkedList::iterator LinkedList::erase(LinkedList::Node& node)
{
    if (first == &node) 
    {
        first = node.next;
        node.root = nullptr;
        node.next = nullptr;
        return begin();
    }
    for(Node& n: *this)
    {
        if (n.next == &node) 
        {
            n.next = node.next;
            node.next = nullptr;
            node.root = nullptr;
            return iterator(n.next);
        }
    }
    return end();
}

void LinkedList::clear()
{
    Node* prev = nullptr;
    for(Node& n: *this)
    {
        if (prev)
            prev->next = nullptr;
        prev = &n;
        n.root = nullptr;
    }
    first = nullptr;
}


LinkedList::iterator LinkedList::insert(const Node& after, Node& node)
{
    if (node.root)
        node.root->erase(node);
    node.next = after.next; 
    node.root = this;
    const_cast<Node&>(after).next = &node;
    return iterator(&node);    
}

LinkedList::iterator LinkedList::insert(const iterator& pos, Node& node)
{
    if (pos==end())
        return push_back(node);
    return insert(*pos, node);
}

std::size_t LinkedList::size() const 
{
    return end()-begin();
}

bool LinkedList::empty() const 
{
    return first==nullptr;
}

}
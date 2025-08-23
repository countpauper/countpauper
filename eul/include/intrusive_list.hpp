#pragma once 
#include <cstdint>
#include <initializer_list>
#include <functional>

namespace eul
{

class intrusive_list 
{
public:


    class node 
    {
    public:
        node();
        node(class intrusive_list& root);
        node(const node& other);
        node(node&& other);
        node& operator=(const node& other);
        node& operator=(node&& other);
        bool linked() const;
        void unlink();
        ~node();
    private:
        friend class intrusive_list;
        intrusive_list* _root;
        mutable class node* _next;
    };

    class iterator 
    {
    public:
        using value_type = node;
        using difference_type = std::ptrdiff_t;
        bool operator==(const iterator& o) const;
        iterator& operator++();
        iterator operator++(int);
        difference_type operator-(const iterator& o) const;
        value_type& operator*() const;
        value_type* operator->() const;
    private:
        friend class intrusive_list;
        iterator();
        iterator(value_type* _node);
        value_type* _node;
    };


    intrusive_list();
    const iterator begin() const;
    const iterator end() const;
    iterator begin();
    iterator end(); 
    node& front();

    iterator push_front(node& _node);
    iterator push_back(node& _node);
    iterator erase(node& _node);
    void clear();
    iterator insert(const iterator& pos, node& _node);
    iterator insert(const node& after, node& _node);
    std::size_t size() const;
    bool empty() const;
protected:
    friend node;
    node* _first=nullptr;
};
}

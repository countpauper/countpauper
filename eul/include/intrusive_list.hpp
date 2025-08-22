#pragma once 
#include <cstdint>
#include <initializer_list>
#include <functional>

namespace eul
{

class LinkedList 
{
public:


    class Node 
    {
    public:
        Node();
        Node(class LinkedList& root);
        Node(const Node& other);
        Node(Node&& other);
        Node& operator=(const Node& other);
        Node& operator=(Node&& other);
        bool IsLinked() const;
        void Unlink();
        ~Node();
    private:
        friend class LinkedList;
        LinkedList* root;
        mutable class Node* next;
    };

    class iterator 
    {
    public:
        using value_type = Node;
        using difference_type = std::ptrdiff_t;
        bool operator==(const iterator& o) const;
        iterator& operator++();
        iterator operator++(int);
        difference_type operator-(const iterator& o) const;
        value_type& operator*() const;
        value_type* operator->() const;
    private:
        friend class LinkedList;
        iterator();
        iterator(value_type* node);
        value_type* node;
    };


    LinkedList();
    const iterator begin() const;
    const iterator end() const;
    iterator begin();
    iterator end(); 
    Node& front();

    iterator push_front(Node& node);
    iterator push_back(Node& node);
    iterator erase(Node& node);
    void clear();
    iterator insert(const iterator& pos, Node& node);
    iterator insert(const Node& after, Node& node);
    std::size_t size() const;
    bool empty() const;
protected:
    friend Node;
    Node* first=nullptr;
};
}
